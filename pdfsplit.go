package main

/*****************************************************************************
 * pdfsplit                                                                  *
 *   pdfsplit is a script to manipulate pdftk into splitting large PDFs      *
 *   on chapter boundaries (1st level bookmarks) into individual chapter     *
 *   files                                                                   *
 *****************************************************************************/

import (
	"bufio"
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"regexp"
	"strconv"
	"strings"
)

type bookmark struct {
	chNum    int
	title    string
	file     string
	level    int
	page     int
	page_end int
}

func (b *bookmark) clear() {
	b.title = ""
	b.file = ""
	b.level = 0
	b.page = 0
	b.chNum = 0
}

/*****************************************************************************
 *                                                                           *
 *                  Relavent DataTags as reported by pdftk                   *
 *                                                                           *
 *****************************************************************************/

var dtPgCnt = "NumberOfPages"
var dtBkmkBegin = "BookmarkBegin"
var dtBkmkTitle = "BookmarkTitle"
var dtBkmkLvl = "BookmarkLevel"
var dtBkmkPg = "BookmarkPageNumber"

var source_file string
var dest_dir string
var pdf_util = "pdftk"

/*****************************************************************************
 *                                                                           *
 *                   Boilerplate utility functions                           *
 *                                                                           *
 *****************************************************************************/

func printUsage() {
	fmt.Println("Usage:\n\tpdfsplit <data> <app>")
}

func init() {
	flag.Parse()
	if (len(flag.Args()) < 1) || (len(flag.Args()) > 2) {
		printUsage()
		os.Exit(2)
	}
	source_file = flag.Arg(0)
	if len(flag.Args()) == 2 {
		dest_dir = flag.Arg(1)
	} else {
		dest_dir, _ = os.Getwd()
	}
}

func fileCloser(f *os.File) {
	if err := f.Close(); err != nil {
		panic(err)
	}
}

/*****************************************************************************
 *                                                                           *
 *                           Main Functions                                  *
 *                                                                           *
 *****************************************************************************/

/*****************************************************************************
 * SanatizeStringForFilesystems
 *
 * @breif Removes or translates characters in the input string that are
 *        fobidden for use in filesystems
 *
 * @param[in] input the string to be sanatized
 * @return sanatized string
 *****************************************************************************/
func SanatizeStringForFilesystem(input string) string {
	input = strings.Replace(input, ".", "_", -1)
	input = strings.Replace(input, " ", "_", -1)
	input = strings.Replace(input, ":", "-", -1)
	input = strings.Trim(input, "\\?=;%~")
	input = strings.Replace(input, "/", "-", -1)

	return input
}

type scanCxt struct {
	foundBookmark bool
	gotTitle      bool
	gotLevel      bool
	gotPage       bool
}

func (self *scanCxt) reset() {
	self.foundBookmark = false
	self.gotTitle = false
	self.gotLevel = false
	self.gotPage = false
}

func (self *scanCxt) finishedBookmark() bool {
	return self.foundBookmark && self.gotTitle && self.gotLevel && self.gotPage
}

/****************************************************************************
 *  ProcessDataTags - processes the data output of pdftk
 *
 *  @param[in] scanner Bufio Scanner that handles the output from the pdftk command
 *  @param[out] pageCount int containing the total number of pages in the pdf
 *  @param[out] chapters bookmark slice containing all the top level bookmarks
 *  @param[out] bookmarks bookmark slice containing all other bookmarks
 ****************************************************************************/
func ProcessDataTags(scanner *bufio.Scanner) (pageCount int, chapters []bookmark, bookmarks []bookmark) {
	var currBkmk bookmark
	bookmarks = make([]bookmark, 0)
	chapters = make([]bookmark, 0)
	matchStr := "^[\\d]+\\.([\\d]+\\.?)* "
	findChNum, err := regexp.Compile(matchStr)
	if err != nil {
		panic(err)
	}

	var state scanCxt
	state.reset()

	for scanner.Scan() {
		line := scanner.Text()
		if strings.Index(line, dtPgCnt) == 0 {
			pageCount, _ = strconv.Atoi(strings.Split(line, " ")[1])
			fmt.Printf("PageCount: %d\n", pageCount)
			if pageCount <= 0 {
				fmt.Printf("No Pages, quitting\n")
				os.Exit(1)
			}
		}
		if strings.Index(line, dtBkmkBegin) == 0 {
			state.reset()
			state.foundBookmark = true
			currBkmk.clear()
		} else if strings.Index(line, dtBkmkTitle) == 0 {
			currBkmk.title = strings.SplitN(line, " ", 2)[1]
			currBkmk.file = currBkmk.title
			if findChNum.Match([]byte(currBkmk.title)) {
				chNum := strings.Split(findChNum.FindString(currBkmk.title), ".")[0]
				currBkmk.chNum, _ = strconv.Atoi(chNum)
			}
			currBkmk.file = SanatizeStringForFilesystem(currBkmk.file)
			state.gotTitle = true
		} else if strings.Index(line, dtBkmkLvl) == 0 {
			level, _ := strconv.Atoi(strings.Split(line, " ")[1])
			currBkmk.level = level
			state.gotLevel = true
		} else if strings.Index(line, dtBkmkPg) == 0 {
			pgnum, _ := strconv.Atoi(strings.Fields(line)[1])
			currBkmk.page = pgnum
			state.gotPage = true
		} else {
			state.reset()
		}
		if state.finishedBookmark() {
			if currBkmk.level == 1 && currBkmk.chNum > 0 {
				if len(chapters) > 0 {
					chapters[len(chapters)-1].page_end = currBkmk.page - 1
				}
				chapters = append(chapters, currBkmk)
			} else {
				bookmarks = append(bookmarks, currBkmk)
			}

			state.reset()
		}
	}
	return pageCount, chapters, bookmarks
}

/****************************************************************************
 * writeChapterInfoFile
 * @breif writes the info file containing all bookmarks for the chapter to be processed
 * @param[in] infoFile the name of the info file to be written
 * @param[in] bkmk the bookmark for the target chapter
 * @param[in] bookmarks the slice on non-chapter bookmarks to source from
 ****************************************************************************/
func writeChapterInfoFile(infoFile string, bkmk bookmark, bookmarks []bookmark) {
	fo, ferr := os.Create(infoFile)
	if ferr != nil {
		panic(ferr)
	}
	defer fileCloser(fo)
	writer := bufio.NewWriter(fo)
	defer writer.Flush()

	fmt.Fprintf(writer, "%s\n", dtBkmkBegin)
	fmt.Fprintf(writer, "%s: %s\n", dtBkmkTitle, bkmk.title)
	fmt.Fprintf(writer, "%s: %d\n", dtBkmkLvl, bkmk.level)
	fmt.Fprintf(writer, "%s: %d\n", dtBkmkPg, 1)
	for _, later := range bookmarks {
		if later.page < bkmk.page {
			continue
		}
		if later.page > bkmk.page_end {
			break
		}
		if later.level > 1 {
			fmt.Printf("%s\n", later.title)

			fmt.Fprintf(writer, "%s\n", dtBkmkBegin)
			fmt.Fprintf(writer, "%s: %s\n", dtBkmkTitle, later.title)
			fmt.Fprintf(writer, "%s: %d\n", dtBkmkLvl, later.level)
			fmt.Fprintf(writer, "%s: %d\n", dtBkmkPg, later.page-bkmk.page+1)
		}
	}
}

func main() {
	cmd := exec.Command(pdf_util, source_file, "dump_data", "output", "-")
	pipe_out, _ := cmd.StdoutPipe()
	pipe_err, _ := cmd.StderrPipe()
	defer pipe_out.Close()

	scanner := bufio.NewScanner(pipe_out)
	errScan := bufio.NewScanner(pipe_err)

	cmd.Start()
	fmt.Printf("starting scan\n")
	pageCount, chapters, bookmarks := ProcessDataTags(scanner)

	for errScan.Scan() {
		fmt.Printf("%v\n", scanner.Text())
	}
	if len(chapters) > 0 {
		chapters[len(chapters)-1].page_end = pageCount
	}
	tmpFile, tmperr := ioutil.TempFile("./", "split_tmp_")
	if tmperr != nil {
		panic(tmperr)
	}
	info, _ := tmpFile.Stat()
	tmpName := info.Name()
	fileCloser(tmpFile)
	defer os.Remove(tmpName)
	for _, bkmk := range chapters {
		if bkmk.level == 1 {
			pageRange := fmt.Sprintf("%d-%d", bkmk.page, bkmk.page_end)
			file := bkmk.file + ".pdf"
			fmt.Printf("Writing: %s\n", file)

			splitCmd := exec.Command(pdf_util, source_file, "cat", pageRange, "output", tmpName)
			_ = splitCmd.Run()

			infoFile := bkmk.file + ".inf"
			writeChapterInfoFile(infoFile, bkmk, bookmarks)

			updateCmd := exec.Command(pdf_util, tmpName, "update_info", infoFile, "output", file)
			_ = updateCmd.Run()
			os.Remove(infoFile)
		}
	}
}

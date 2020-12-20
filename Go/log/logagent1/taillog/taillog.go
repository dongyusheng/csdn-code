package taillog

import (
	"fmt"
	"github.com/hpcloud/tail"
)

var (
	tails *tail.Tail
)

func Init(filename string) (err error) {
	// 初始化配置
	config := tail.Config{
		ReOpen: true,
		Follow: true,
		Location: &tail.SeekInfo{ Offset: 0, Whence: 2 },
		MustExist: false,
		Poll: true,
	}

	// 跟踪日志文件
	tails, err = tail.TailFile(filename, config)
	if err != nil{
		fmt.Println("tail file error: ", err)
		return
	}

	return nil
}

func GetTailsLine() <- chan *tail.Line {
	return tails.Lines
}
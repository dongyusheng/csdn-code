package main

import (
	"fmt"
	"code.dongshao.com/logagent/kafka"
	"code.dongshao.com/logagent/taillog"
	"code.dongshao.com/logagent/conf"
	"gopkg.in/ini.v1"
	"time"
)

var (
	cfg = new(conf.AppConf)
)

func main() {
	// 加载配置文件
	if err := ini.MapTo(cfg, "./conf/conf.ini"); err != nil {
		fmt.Println("load ini file error: ", err)
		return
	}

	// 初始化kafka生产者
	fmt.Println("Address: ", cfg.KafkaConf.Address)
	if err := kafka.Init([]string{ cfg.KafkaConf.Address }); err != nil {
		fmt.Println("init kafka error: ", err)
		return
	}

	// 初始化监听日志文件
	fmt.Println("filename: ", cfg.TailLogConf.FileName)
	if err := taillog.Init(cfg.TailLogConf.FileName); err != nil {
		fmt.Println("init taillog error: ", err)
		return
	}

	// 启动服务
	Run()
}

func Run() {
	for {
		select {
		// 监听日志数据, 有数据了之后传递给kafka生产者, 让其传递给kafka消费者
		case data, b := <- taillog.GetTailsLine():
			if !b {
				fmt.Println("send data to kafka error")
				return
			}
			if err := kafka.SendMessage([]byte(data.Text)); err != nil {
				return
			}
		default:
			time.Sleep(1 * time.Second)
		}
	}
}
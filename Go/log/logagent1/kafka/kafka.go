package kafka

import (
	"fmt"
	"github.com/Shopify/sarama"
)

var (
	procuder sarama.SyncProducer
)

// 初始化kafka服务
func Init(addrs []string) (err error) {
	// 初始化配置
	config := sarama.NewConfig()
	config.Producer.RequiredAcks = sarama.WaitForAll
	config.Producer.Partitioner = sarama.NewRandomPartitioner
	config.Producer.Return.Successes = true

	// 创建生产者
	procuder, err = sarama.NewSyncProducer(addrs, config)
	if err != nil {
		fmt.Println("create kafka producer error: ", err)
		return err
	}

	return nil
}

func SendMessage(data []byte) error {
	// 构造消息
	msg := &sarama.ProducerMessage{}
	msg.Topic = "web_log"
	msg.Value = sarama.StringEncoder(data)

	// 发送消息
	pid, offset, err := procuder.SendMessage(msg)
	if err != nil {
		fmt.Println("producer sendMessage error: ", err)
		return err
	}
	fmt.Println("pid:%v offset:%v\n", pid, offset)

	return nil
}
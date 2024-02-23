package main

/*
#cgo pkg-config: libqalculate
#cgo CXXFLAGS: -std=gnu++11
#include <stdio.h>
#include <stdlib.h>
#include "qalc.h"
*/
import "C"
import (
	"regexp"
	"strings"
	"time"
	"unsafe"

	"github.com/whyrusleeping/hellabot"

	log "gopkg.in/inconshreveable/log15.v2"
)

var logHandler = log.LvlFilterHandler(log.LvlInfo, log.StdoutHandler)
var srvlog = log.New("module", "qalcbot")

var colors = strings.NewReplacer(
	"\033[0;30m", "\x0301", // BLACK
	"\033[0;31m", "\x0304", // RED
	"\033[0;32m", "\x0303", // GREEN
	"\033[0;33m", "\x0308", // YELLOW
	"\033[0;34m", "\x0312", // BLUE
	"\033[0;35m", "\x0313", // MAGENTA
	"\033[0;36m", "\x0311", // CYAN
	"\033[0;37m", "\x0300", // WHITE
	"\033[0m", "\x03",
)

var colorStrip = regexp.MustCompile(`\033[0-9\[;]*m`)

func qalculate(in string) string {

	er_updated := int64(C.exchange_rates_time())

	if time.Now().Unix()-er_updated > 7200 {
		C.update_exchange_rates()
		srvlog.Info("Updating exchange rates...", "last_updated", er_updated)
	}

	name := C.CString(in)
	defer C.free(unsafe.Pointer(name))

	ptr := C.malloc(C.sizeof_char * 1024)
	defer C.free(unsafe.Pointer(ptr))

	size := C.qalculate(name, (*C.char)(ptr), C.int(1024), 1)

	b := C.GoBytes(ptr, size)
	s := colors.Replace(string(b))

	return colorStrip.ReplaceAllString(s, "")

}

func main() {

	srvlog.Info("Initializing qalculate...")
	C.initialize_qalc()

	srvlog.Info("Configuring bot...")
	var qalcTrigger = hbot.Trigger{
		func(b *hbot.Bot, m *hbot.Message) bool {
			if len(m.Content) <= 6 {
				return false
			}
			if m.Content[0:6] != "qalc: " {
				return false
			}
			switch strings.ToLower(m.From) {
			case
				"bucket",
				"isaac",
				"dice",
				"unitsbot":
				return false
			}
			return true
		},

		func(b *hbot.Bot, m *hbot.Message) bool {
			b.Reply(m, m.From+": "+qalculate(m.Content[6:]))
			return false
		},
	}

	botOptions := func(bot *hbot.Bot) {
		bot.SSL = true
		bot.Channels = []string{"#bot-test-t486"}
	}

	mybot, err := hbot.NewBot("irc.slashnet.org:6697", "qalc", botOptions)
	if err != nil {
		panic(err)
	}

	srvlog.Info("Connecting to Network...")

	mybot.Logger.SetHandler(logHandler)

	mybot.AddTrigger(qalcTrigger)
	mybot.Run()

}

#!/usr/bin/expect
if { $argc == 3 } {
	set  routerip [lindex $argv 0]
	set clientip [lindex $argv 1]
	set clientport [lindex $argv 2]
} else { puts "Usage: set_client.tcl routerip clientip clientport!\n" ; exit 1}

set timeout 5
set password 123
set infofile ./netstream_info.txt

#获取最新配置文件
expect get_info.tcl $routerip
set fil [open $infofile r]

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "system-view\r"
expect "]"
while {[gets $fil line ]>=0} {
	incr n
	set pos [string first host $line]
	if { $pos != -1 } {
		send "undo $line\r"
		break;
	} else {}
}
send "ip netstream export host $clientip $clientport\r"
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect "]"


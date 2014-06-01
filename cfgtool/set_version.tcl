#!/usr/bin/expect
if { $argc == 2 } {
	set  routerip [lindex $argv 0]
	set version [lindex $argv 1]
} else { puts "Usage: set_version.tcl routerip versionid(5/8/9)!\n" ; exit 1}

set timeout 5
set password 123

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "system-view\r"
expect "]"
send "ip netstream export version $version\r"
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect "]"


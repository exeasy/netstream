#!/usr/bin/expect
if { $argc == 3 } {
	set routerip [lindex $argv 0]
	set type [lindex $argv 1]
	set value [lindex $argv 2]
} else { puts "Usage: set_timeout.tcl routerip type value!\n" }

set timeout 5
set password 123

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "system-view\r"
expect "]"
send "ip netstream timeout $type $value\r"
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect "]"

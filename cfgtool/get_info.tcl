#!/usr/bin/expect
if { $argc == 1 } {
	set routerip [lindex $argv 0]
} else { puts "Usage: get_info.tcl routerip!\n" ; exit 1}

set timeout 5
set password 123
set loop 1

spawn telnet $routerip
expect "*Password*"
send "$password\r"
expect ">"
send "screen-length 0 temporary\r"
expect "]"
send "system-view\r"
expect "]"
send "display ip netstream all\r"
log_file -noappend netstream_info.txt
while { $loop > 0 } {
	expect {
		"*Press 'Q' to break*" { send "\r"; }
		"]" { set loop 0; }
	}
}
log_user 0
expect "]"
send "quit\r"
expect ">"
send "quit\r"
expect eof
exit 0


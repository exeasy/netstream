# -*- coding: utf-8 -*-
import ConfigParser
import os
cfgdir="./"

def load_cfg():
    config = ConfigParser.ConfigParser()
    config.readfp( open('../cfg.ini'))
    routerip = config.get("Router","ip")
    version = config.get("NetStream","version")
    os.system("expect "+cfgdir+"set_version.tcl "+routerip+" "+version)

    
    clientip = config.get("Client","ip")
    clientport = config.get("Client", "port")
    os.system("expect "+cfgdir+"set_client.tcl "+routerip+" "+clientip+" "+clientport)

    inactive = config.get("timeout","inactive")
    active = config.get("timeout", "active")
    tcpflag = config.get("timeout", "tcpflag")
    os.system("expect "+cfgdir+"set_timeout.tcl "+routerip+" active "+active)
    os.system("expect "+cfgdir+"set_timeout.tcl "+routerip+" inactive "+inactive)
    os.system("expect "+cfgdir+"set_tcpflag.tcl "+routerip+" "+tcpflag)

    if config.has_section("Interface0"):
        stype = config.get("Interface0", "sample_type")
        svalue = config.get("Interface0", "value")
        os.system("expect "+cfgdir+"set_sampler.tcl "+routerip+" 0 "+stype+" "+svalue)
    if config.has_section("Interface1"):
        stype = config.get("Interface1", "sample_type")
        svalue = config.get("Interface1", "value")
        os.system("expect "+cfgdir+"set_sampler.tcl "+routerip+" 1 "+stype+" "+svalue)
    if config.has_section("Interface2"):
        stype = config.get("Interface2", "sample_type")
        svalue = config.get("Interface2", "value")
        os.system("expect "+cfgdir+"set_sampler.tcl "+routerip+" 2 "+stype+" "+svalue)

load_cfg()


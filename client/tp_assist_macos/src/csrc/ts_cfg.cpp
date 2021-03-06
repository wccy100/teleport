#include "ts_cfg.h"
#include "ts_env.h"

TsCfg g_cfg;

TsCfg::TsCfg()
{}

TsCfg::~TsCfg()
{}

bool TsCfg::init(void) {
	ex_astr file_content;
	if(!ex_read_text_file(g_env.m_cfg_file, file_content)) {
		EXLOGE("can not load config file.\n");
		return false;
	}
	
	if(!_load(file_content))
		return false;

    if(!m_root.isObject()) {
        EXLOGE("invalid config file, not in json format?\n");
        return false;
    }
    
    if(m_root["file_version"].isNull()) {
        EXLOGE("invalid config file, maybe need create new one?\n");
        return false;
    }
    
    if(!m_root["file_version"].isInt()) {
        
    }
    
	return true;
}

bool TsCfg::save(const ex_astr& new_value)
{
	if(!_load(new_value))
		return false;
	
	Json::StyledWriter jwriter;
	ex_astr val = jwriter.write(m_root);
	
	if(!ex_write_text_file(g_env.m_cfg_file, val)) {
		EXLOGE("can not save config file.\n");
		return false;
	}

	return true;
}

bool TsCfg::_parse_app(const Json::Value& m_root, const ex_astr& str_app, APP_CONFIG& cfg) {
    const Json::Value& jApp = m_root[str_app.c_str()];
    if(!jApp.isObject())
        return false;
    
    if (!jApp["selected"].isString()) {
        EXLOGE("invalid config, error 2.\n");
        return false;
    }
    ex_astr _selected = jApp["selected"].asCString();;

    if (!jApp["available"].isArray() || jApp["available"].size() == 0) {
        EXLOGE("invalid config, error 3.\n");
        return false;
    }
    const Json::Value& jAppList = jApp["available"];

    int i = 0;
    for (i = 0; i < jAppList.size(); ++i) {
        if (
            !jAppList[i]["name"].isString()
            || !jAppList[i]["app"].isString()
            || !jAppList[i]["cmdline"].isString()
            || !jAppList[i]["desc"].isArray()
            ) {
            EXLOGE("invalid config, error 4.\n");
            return false;
        }
        
        if(jAppList[i]["name"].asString().empty()) {
            EXLOGE("invalid config, need name.\n");
            return false;
        }

        if (jAppList[i]["display"].isNull() || jAppList[i]["display"].asString().empty()) {
            cfg.display = jAppList[i]["name"].asCString();
        } else
            cfg.display = jAppList[i]["display"].asCString();

        if (jAppList[i]["name"].asCString() != _selected)
            continue;
        
        cfg.name = jAppList[i]["name"].asCString();
        cfg.display = jAppList[i]["display"].asCString();
        cfg.application = jAppList[i]["app"].asCString();
        cfg.cmdline = jAppList[i]["cmdline"].asCString();

        if(jAppList[i]["desc"].size() > 0) {
            const Json::Value& jAppDescList = jAppList[i]["desc"];
            
            int j = 0;
            for(j = 0; j < jAppDescList.size(); ++j) {
                if(!jAppDescList[j].isString())
                    return false;
                cfg.description.push_back(jAppDescList[j].asCString());
            }
        }

        break;
    }
    
//    if (cfg.application.empty() || cfg.cmdline.empty()) {
//        EXLOGE("invalid config, error 6.\n");
//        return false;
//    }

    return true;
}


bool TsCfg::_load(const ex_astr& str_json) {
	Json::Reader jreader;

	if (!jreader.parse(str_json.c_str(), m_root)) {
		EXLOGE("can not parse new config data, not in json format?\n");
		return false;
	}

	//===================================
	// check ssh config
	//===================================
    if(!_parse_app(m_root, "ssh", ssh))
        return false;
    if(!_parse_app(m_root, "sftp", sftp))
        return false;
//    if(!_parse_app(m_root, "telnet", telnet))
//        return false;
    if(!_parse_app(m_root, "rdp", rdp))
        return false;

#if 0
	if (!m_root["ssh"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if (!m_root["ssh"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}
	
	sel_name = m_root["ssh"]["selected"].asCString();
	
	if (!m_root["ssh"]["available"].isArray() || m_root["ssh"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}
	
	for (i = 0; i < m_root["ssh"]["available"].size(); ++i) {
		
		if (
			!m_root["ssh"]["available"][i]["name"].isString()
			|| !m_root["ssh"]["available"][i]["app"].isString()
			|| !m_root["ssh"]["available"][i]["cmdline"].isString()
			) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if (m_root["ssh"]["available"][i]["display"].isNull()) {
			m_root["ssh"]["available"][i]["display"] = m_root["ssh"]["available"][i]["name"];
		}
		
		if (m_root["ssh"]["available"][i]["name"].asCString() != sel_name)
			continue;
		
		ssh_app = m_root["ssh"]["available"][i]["app"].asCString();
		ssh_cmdline = m_root["ssh"]["available"][i]["cmdline"].asCString();
		
		break;
	}
	
	if (ssh_app.length() == 0 || ssh_cmdline.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}
	
	
	//===================================
	// check sftp config
	//===================================
	
	if (!m_root["scp"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if (!m_root["scp"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}
	
	sel_name = m_root["scp"]["selected"].asCString();
	
	if (!m_root["scp"]["available"].isArray() || m_root["scp"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}
	
	for (i = 0; i < m_root["sftp"]["available"].size(); ++i) {
		
		if (
			!m_root["sftp"]["available"][i]["name"].isString()
			|| !m_root["sftp"]["available"][i]["app"].isString()
			|| !m_root["sftp"]["available"][i]["cmdline"].isString()
			) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if (m_root["scp"]["available"][i]["display"].isNull()) {
			m_root["scp"]["available"][i]["display"] = m_root["scp"]["available"][i]["name"];
		}
		
		if (m_root["scp"]["available"][i]["name"].asCString() != sel_name)
			continue;
		
//        tmp = m_root["scp"]["available"][i]["app"].asCString();
//        ex_astr2wstr(tmp, scp_app, EX_CODEPAGE_UTF8);
        scp_app = m_root["scp"]["available"][i]["app"].asCString();
//        tmp = m_root["scp"]["available"][i]["cmdline"].asCString();
//        ex_astr2wstr(tmp, scp_cmdline, EX_CODEPAGE_UTF8);
        scp_cmdline = m_root["scp"]["available"][i]["cmdline"].asCString();

		break;
	}
	
	if (scp_app.length() == 0 || scp_cmdline.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}
	
	//===================================
	// check telnet config
	//===================================
	
	if (!m_root["telnet"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if (!m_root["telnet"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}
	
	sel_name = m_root["telnet"]["selected"].asCString();
	
	if (!m_root["telnet"]["available"].isArray() || m_root["telnet"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}
	
	for (i = 0; i < m_root["telnet"]["available"].size(); ++i) {
		
		if (
			!m_root["telnet"]["available"][i]["name"].isString()
			|| !m_root["telnet"]["available"][i]["app"].isString()
			|| !m_root["telnet"]["available"][i]["cmdline"].isString()
			) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if (m_root["telnet"]["available"][i]["display"].isNull()) {
			m_root["telnet"]["available"][i]["display"] = m_root["telnet"]["available"][i]["name"];
		}
		
		if (m_root["telnet"]["available"][i]["name"].asCString() != sel_name)
			continue;
		
//        tmp = m_root["telnet"]["available"][i]["app"].asCString();
//        ex_astr2wstr(tmp, telnet_app, EX_CODEPAGE_UTF8);
//        tmp = m_root["telnet"]["available"][i]["cmdline"].asCString();
//        ex_astr2wstr(tmp, telnet_cmdline, EX_CODEPAGE_UTF8);
        telnet_app = m_root["telnet"]["available"][i]["app"].asCString();
        telnet_cmdline = m_root["telnet"]["available"][i]["cmdline"].asCString();

		break;
	}
	
	if (telnet_app.length() == 0 || telnet_cmdline.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}
	
	//===================================
	// check rdp config
	//===================================
	
	if (!m_root["rdp"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if (!m_root["rdp"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}
	
	sel_name = m_root["rdp"]["selected"].asCString();
	
	if (!m_root["rdp"]["available"].isArray() || m_root["rdp"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}
	
	for (i = 0; i < m_root["rdp"]["available"].size(); ++i) {
		
		if (
			!m_root["rdp"]["available"][i]["name"].isString()
			|| !m_root["rdp"]["available"][i]["app"].isString()
			|| !m_root["rdp"]["available"][i]["cmdline"].isString()
			) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if (m_root["rdp"]["available"][i]["display"].isNull()) {
			m_root["rdp"]["available"][i]["display"] = m_root["rdp"]["available"][i]["name"];
		}
		
		if (m_root["rdp"]["available"][i]["name"].asCString() != sel_name)
			continue;
		
//        tmp = m_root["rdp"]["available"][i]["app"].asCString();
//        ex_astr2wstr(tmp, rdp_app, EX_CODEPAGE_UTF8);
//        tmp = m_root["rdp"]["available"][i]["cmdline"].asCString();
//        ex_astr2wstr(tmp, rdp_cmdline, EX_CODEPAGE_UTF8);
//        tmp = m_root["rdp"]["available"][i]["name"].asCString();
//        ex_astr2wstr(tmp, rdp_name, EX_CODEPAGE_UTF8);
        rdp_app = m_root["rdp"]["available"][i]["app"].asCString();
        rdp_cmdline = m_root["rdp"]["available"][i]["cmdline"].asCString();
        rdp_name = m_root["rdp"]["available"][i]["name"].asCString();

		break;
	}
	
	if (rdp_app.length() == 0 || rdp_cmdline.length() == 0 || rdp_name.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}
#endif
	
#if 0
	// ------------ term ---------------------
	
	if (!m_root["term"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if(	!m_root["term"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}

	term_name = m_root["term"]["selected"].asCString();
	
	if(!m_root["term"]["available"].isArray() || m_root["term"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}

	int i = 0;
	for (i = 0; i < m_root["term"]["available"].size(); ++i) {

		if(
		   !m_root["term"]["available"][i]["name"].isString()
		   || !m_root["term"]["available"][i]["app"].isString()
		   || !m_root["term"]["available"][i]["profile"].isString()
		   ) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if(m_root["term"]["available"][i]["name"].asCString() != term_name)
			continue;

		if(m_root["term"]["available"][i]["disp"].isString()) {
			term_display = m_root["term"]["available"][i]["display"].asCString();
		} else if(m_root["term"]["available"][i]["disp"].isNull()) {
			m_root["term"]["available"][i]["disp"] = term_name;
			term_display = term_name;
		} else {
			EXLOGE("invalid config, error 5.\n");
			return false;
		}
		
		term_app = m_root["term"]["available"][i]["app"].asCString();
		term_profile = m_root["term"]["available"][i]["profile"].asCString();

		break;
	}

	if(term_name.length() == 0 || term_app.length() == 0 || term_profile.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}

	// ------------ RDP ---------------------

	if (!m_root["rdp"].isObject()) {
		EXLOGE("invalid config, error 1.\n");
		return false;
	}
	
	if(	!m_root["rdp"]["selected"].isString()) {
		EXLOGE("invalid config, error 2.\n");
		return false;
	}
	
	rdp_name = m_root["rdp"]["selected"].asCString();
	
	if(!m_root["rdp"]["available"].isArray() || m_root["rdp"]["available"].size() == 0) {
		EXLOGE("invalid config, error 3.\n");
		return false;
	}

	for (i = 0; i < m_root["rdp"]["available"].size(); ++i) {
		
		if(
		   !m_root["rdp"]["available"][i]["name"].isString()
		   || !m_root["rdp"]["available"][i]["app"].isString()
		   //|| !m_root["rdp"]["available"][i]["profile"].isString()
		   ) {
			EXLOGE("invalid config, error 4.\n");
			return false;
		}
		
		if(m_root["rdp"]["available"][i]["name"].asCString() != rdp_name)
			continue;
		
		if(m_root["rdp"]["available"][i]["disp"].isString()) {
			rdp_display = m_root["term"]["available"][i]["display"].asCString();
		} else if(m_root["rdp"]["available"][i]["disp"].isNull()) {
			m_root["rdp"]["available"][i]["disp"] = rdp_name;
			rdp_display = rdp_name;
		} else {
			EXLOGE("invalid config, error 5.\n");
			return false;
		}
		
		rdp_app = m_root["rdp"]["available"][i]["app"].asCString();
		//rdp_profile = m_root["rdp"]["available"][i]["profile"].asCString();
		
		break;
	}
	
	if(rdp_name.length() == 0) {
		EXLOGE("invalid config, error 6.\n");
		return false;
	}
#endif

	return true;
}

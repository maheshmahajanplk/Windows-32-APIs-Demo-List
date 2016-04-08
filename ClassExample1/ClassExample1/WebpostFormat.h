
#include<iostream>
#include<string>

class WebpostFormat
{
public:
	std::string szAppId;       /*<wpappmondata><device><regid><dtime><apid><port><prs><svc><wexcode><svexcode><scmcode><status><startuptype><svcstate>*/
	std::string szPort;
	std::string szProcess;
	std::string szService;
	std::string szWexcode;
	std::string szSvexcode;
	std::string szScmexcode;
	std::string szStatus;
	std::string szStartupType;
	std::string szSvcstate;

	//default constructor.....
	WebpostFormat(): szAppId(""),
				     szPort(""),
					 szProcess(""),
					 szService(""),
					 szWexcode(""),
					 szSvexcode(""),
					 szScmexcode(""),
					 szStatus(""),
					 szStartupType(""),
					 szSvcstate("")
	{}
				  

		         

	WebpostFormat(std::string const& appid,
		          std::string const& port,
				  std::string const& prs,
				   std::string const& svc,
				   std::string const& wexcode,
				   std::string const& svexcode,
				   std::string const& scmexcode,
				   std::string const& status,
				   std::string const& stype,
				   std::string const& svcstate) : szAppId(appid),
											 szPort(port),
											 szProcess(prs),
									         szService(svc),
									         szWexcode(wexcode),
									         szSvexcode(svexcode),
									         szScmexcode(scmexcode),
									         szStatus(status),
									         szStartupType(stype),
									         szSvcstate(svcstate)
	{	}

	//destructor...
	~WebpostFormat(){
	}

	std::string getStatus()
	{
		return szStatus; 
	}
	/******Setter Methods****************/

	void setAppID(char *appid)
	{
		szAppId.assign(appid);
	}

	void setPort(char *port)
	{
		szPort.assign(port);
	}

	void setProcessName(char * prs)
	{
		szProcess.assign(prs);
	}

	void setServiceName(char *svc)
	{
		szService.assign(svc);
	}

	void setWindowsExCode(char *wexcode)
	{
		szWexcode.assign(wexcode);
	}

	void setServiceExCode(char *svexcode)
	{
		szSvexcode.assign(svexcode);
	}

	void setScmCode(char *scmexcode) 
	{
		szScmexcode.assign(scmexcode);
	}

	void setStatus(char *status)
	{
		szStatus.assign(status);
	}

	void setStartUpType(char *startuptype)
	{
		szStartupType.assign(startuptype);
	}

	void setServiceState(char *svcstate)
	{
		szSvcstate.assign(svcstate);
	}
		           
};
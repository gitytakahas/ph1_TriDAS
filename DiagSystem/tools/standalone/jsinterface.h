#ifndef _JsInterface_h
#define _JsInterface_h
#include "toolbox/rlist.h"
#include "toolbox/mem/Reference.h"
#include "toolbox/lang/Class.h"
#include "i2o/utils/AddressMap.h"
#include "xdaq/WebApplication.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/ApplicationContext.h"
#include "xdata/UnsignedShort.h"
#include "xdata/Boolean.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"
#include "xdata/String.h"

// To be modified with new Event Builder

using namespace std;

class JsInterface  : public virtual toolbox::lang::Class
{
public:
  JsInterface(xdaq::ApplicationStub* app)
  {
    app_=app;
    xgi::bind(this,&JsInterface::handleApplicationRequest,"apprequest");
    xgi::bind(this,&JsInterface::handleRequest,"request");
    xgi::bind(this,&JsInterface::handleFuncRequest,"requestF");
    xgi::bind(this,&JsInterface::handlePostRequest,"postrequest");
    

    std::set<std::string> zones = app_->getContext()->getZoneNames();
    for ( std::set<std::string>::iterator i = zones.begin(); i != zones.end(); i++ )
        {
           std::set<xdaq::ApplicationGroup *>  groups = app_->getContext()->getZone(*i)->getGroups();

           for ( std::set<xdaq::ApplicationGroup *>::iterator j = groups.begin(); j != groups.end(); j++ )
                {
                        std::set<xdaq::ApplicationDescriptor*> descriptors = (*j)->getApplicationDescriptors ();
                        merged_.insert(descriptors.begin(), descriptors.end());
                }
        }

  }
  
  void handleApplicationRequest(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{   
  try 
    {
      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);
      const cgicc::CgiEnvironment& env = cgi.getEnvironment();
      std::string qString = cgicc::form_urldecode(env.getQueryString());
      cout << "Posted Data " << cgicc::form_urldecode(env.getPostData()) << endl;
      cerr << qString << endl;
      cout << "Service : " <<app_->getDescriptor()->getAttribute("service") << endl;
      
      std::ostringstream xmlstr;
#ifdef USEXML
      out->getHTTPResponseHeader().addHeader("Content-Type", "text/xml");
       xmlstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
       xmlstr << "<response success=\"true\">" << endl;
       
        for (std::set<xdaq::ApplicationDescriptor*>::iterator i = merged_.begin(); i != merged_.end(); i++)
        {
                xdaq::ApplicationDescriptor* d = *i;
	  xmlstr<<"<XdaqApplication> "<<endl;
	  xmlstr<<"<Url> "<<d->getContextDescriptor()->getURL()<<"</Url> "<<endl;
	  xmlstr<<"<Urn>/urn:xdaq-application:lid="<<d->getLocalId()<<"</Urn> "<<endl;
	  xmlstr<<"<Class>"<<d->getClassName()<<"</Class>"<<endl;
	   if ( d->hasInstanceNumber() ) {
	   xmlstr<<"<Instance>"<<d->getInstance()<<"</Instance>"<<endl;
                }
	xmlstr<<"</XdaqApplication> "<<endl;	
      }
      xmlstr << "</response>" ;
#else
      // "[{\"url\":\"http://lxcms01:40000\", \"urn\":\"/urn:xdaq-application:lid=10\",\"Class\":\"FE\",\"instance\":\"0\"},{\"url\":\"http://lxcms01:40000\", \"urn\":\"/urn:xdaq-application:lid=11\",\"Class\":\"FE\",\"instance\":\"1\"},{\"url\":\"http://lxcms01:40000\", \"urn\":\"/urn:xdaq-application:lid=12\",\"Class\":\"Fed9USupervisor\",\"instance\":\"0\"}]";

      xmlstr <<"[";
      for (std::set<xdaq::ApplicationDescriptor*>::iterator i = merged_.begin(); i != merged_.end(); i++)
        {
	  if (i!=merged_.begin()) xmlstr<<",";
	  xdaq::ApplicationDescriptor* d = *i;
	  
		
	  xmlstr<<"{";
	  xmlstr<<"\"url\":\""<<d->getContextDescriptor()->getURL()<<"\",";
	  xmlstr<<"\"urn\":\"/urn:xdaq-application:lid="<<d->getLocalId()<<"\",";
	  xmlstr<<"\"Class\":\""<<d->getClassName()<<"\"";
	   if ( d->hasInstanceNumber() ) {
	   xmlstr<<",\"instance\":\""<<d->getInstance()<<"\"";
                }
	   xmlstr<<"}";
	}
      xmlstr<<"]";
#endif


      *out << xmlstr.str();
      

    }
  
 catch(const std::exception& e) 
    {
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
}

  void handleRequest(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{   
  try 
    {
      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);
      const cgicc::CgiEnvironment& env = cgi.getEnvironment();
      std::string qString = cgicc::form_urldecode(env.getQueryString());
      cout << "Posted Data " << cgicc::form_urldecode(env.getPostData()) << endl;
      cerr << qString << endl;
      cout << "Service : " <<app_->getDescriptor()->getAttribute("service") << endl;
      
      std::vector<cgicc::FormEntry> entries = cgi.getElements();

 
 

      std::string CallBackName="NONE";

      for (unsigned int i = 0; i < entries.size(); i++)
	{
	  if (entries[i].getName()=="callback")
	    {
	    std::cout<<"Calback name = "<<entries[i].getValue()<<std::endl;
	    CallBackName=entries[i].getValue();
	    }
	}

      std::ostringstream xmlstr;
      //paramList+="{\"Legend\":\" une string \",\"Name\":\"Param1\",\"Type\":\"string\",\"Value\":\"Salut Les gars vogon styr dans le guide du routard galactique\"},";
      if (CallBackName=="NONE")
	xmlstr <<"[";
      else
	xmlstr<<CallBackName<<"( {\"params\":[";
      bool first=true;
      std::map<std::string,xdata::Serializable*> mparam = app_->getInfoSpace()->match(".*");
       for ( std::map<std::string,xdata::Serializable*>::iterator i=mparam.begin();i!=mparam.end();i++)
	 {
	   if (i->second->type() =="properties") continue;
	    if (!first) xmlstr<<",";
	   if (first) first=false;
	   std::string groupname =(group_.count(i->first)!=0)?group_[i->first]:"default";
	   std::string legend =(legend_.count(i->first)!=0)?legend_[i->first]:"Not Available";
	   std::string modif = (modifiable_.count(i->first)!=0)?modifiable_[i->first]:"true";
	   xmlstr<<"{";
	  xmlstr<<"\"Legend\":\""<<legend<<"\",";
	  xmlstr<<"\"Group\":\""<<groupname<<"\",";
	  xmlstr<<"\"Change\":\""<<modif<<"\",";
	  xmlstr<<"\"Name\":\""<<i->first<<"\","<<endl;
	  xmlstr<<"\"Type\":\""<<i->second->type()<<"\",";
	  xmlstr<<"\"Value\":\""<<i->second->toString()<<"\"";
	   xmlstr<<"}";
	 }
       if (CallBackName=="NONE")
	 xmlstr<<"]";
       else
	 xmlstr<<"]});";
	 std::cout<<xmlstr.str()<<std::endl;



      *out << xmlstr.str();
      

    }
  
 catch(const std::exception& e) 
    {
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
}

  void handleFuncRequest(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{   
  try 
    {
      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);
      const cgicc::CgiEnvironment& env = cgi.getEnvironment();
      std::string qString = cgicc::form_urldecode(env.getQueryString());
      cout << "Posted Data " << cgicc::form_urldecode(env.getPostData()) << endl;
      cerr << qString << endl;
      cout << "Service : " <<app_->getDescriptor()->getAttribute("service") << endl;

      std::vector<cgicc::FormEntry> entries = cgi.getElements();

 
 

      std::string ParamName,ParamType,ParamValue;

      for (unsigned int i = 0; i < entries.size(); i++)
	{
	  if (entries[i].getName()=="callback")
	    std::cout<<"Calback name = "<<entries[i].getValue()<<std::endl;
	}
      
      std::ostringstream xmlstr;

      //paramList+="{\"Legend\":\" une string \",\"Name\":\"Param1\",\"Type\":\"string\",\"Value\":\"Salut Les gars vogon styr dans le guide du routard galactique\"},";

      xmlstr <<"jsonCallback( {\"params\":[";
      bool first=true;
      std::map<std::string,xdata::Serializable*> mparam = app_->getInfoSpace()->match(".*");
       for ( std::map<std::string,xdata::Serializable*>::iterator i=mparam.begin();i!=mparam.end();i++)
	 {
	   if (i->second->type() =="properties") continue;
	    if (!first) xmlstr<<",";
	   if (first) first=false;
	   std::string groupname =(group_.count(i->first)!=0)?group_[i->first]:"default";
	   std::string legend =(legend_.count(i->first)!=0)?legend_[i->first]:"Not Available";
	   std::string modif = (modifiable_.count(i->first)!=0)?modifiable_[i->first]:"true";
	   xmlstr<<"{";
	  xmlstr<<"\"Legend\":\""<<legend<<"\",";
	  xmlstr<<"\"Group\":\""<<groupname<<"\",";
	  xmlstr<<"\"Change\":\""<<modif<<"\",";
	  xmlstr<<"\"Name\":\""<<i->first<<"\","<<endl;
	  xmlstr<<"\"Type\":\""<<i->second->type()<<"\",";
	  xmlstr<<"\"Value\":\""<<i->second->toString()<<"\"";
	   xmlstr<<"}";
	 }
         xmlstr<<"]});";
	 std::cout<<xmlstr.str()<<std::endl;


      *out << xmlstr.str();
      

    }
  
 catch(const std::exception& e) 
    {
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
}

void handlePostRequest(xgi::Input * in, xgi::Output * out) throw (xgi::exception::Exception)
{   
  try 
    {
      // Create a new Cgicc object containing all the CGI data
      cgicc::Cgicc cgi(in);
      const cgicc::CgiEnvironment& env = cgi.getEnvironment();
      std::string qString = cgicc::form_urldecode(env.getQueryString());
      cout << "Posted Data " << cgicc::form_urldecode(env.getPostData()) << endl;
      cout << "Posted " << env.getPostData() <<endl;
      cout << "Type " << env.getContentType() <<endl;
      cout << "Query string " << env.getQueryString() <<endl;
      cout << "Method " << env.getRequestMethod() <<endl;
      //      cerr << qString << endl;


      std::vector<cgicc::FormEntry> entries = cgi.getElements();

 
 

      std::string ParamName,ParamType,ParamValue;

      for (unsigned int i = 0; i < entries.size(); i++)
	{
	 ParamName  = entries[i].getName();
	 ParamValue = entries[i].getValue();

	  std::cout << "Read " << ParamName << " = " << ParamValue << std::endl;
	  std::map<std::string,xdata::Serializable*> mparam = app_->getInfoSpace()->match(ParamName);
	  for ( std::map<std::string,xdata::Serializable*>::iterator i=mparam.begin();i!=mparam.end();i++)
	    {
	      std::cout<< i->first << std::endl;
	      if ( i->second->type() == "string")
		{
		  xdata::String* s = (xdata::String*) i->second;
		  s->fromString(ParamValue);
		}
	      if (  i->second->type() == "bool")
		{
		  xdata::Boolean* s = (xdata::Boolean*) i->second;
		  s->fromString(ParamValue);
		}
	      if (  i->second->type() == "int")
		{
		  xdata::Integer* s = (xdata::Integer*) i->second;
		  s->fromString(ParamValue);
		}
	      if (  i->second->type() == "unsigned long")
		{
		  xdata::UnsignedLong* s = (xdata::UnsignedLong*) i->second;
		  s->fromString(ParamValue);
		}
	      if (  i->second->type() == "unsigned short")
		{
		  xdata::UnsignedShort* s = (xdata::UnsignedShort*) i->second;
		  s->fromString(ParamValue);
		}

	}
      
	}



 }
  
 catch(const std::exception& e) 
    {
      XCEPT_RAISE(xgi::exception::Exception,  e.what());
    }
}

void declareParameter(std::string name,std::string legend="No description",std::string group="default",bool modifiable=true)
{
  legend_[name] = legend;
  group_[name] = group;
  modifiable_[name] =modifiable?"true":"false"; 
}
void declareParameter(xdaq::Application* app,std::string name,xdata::Serializable* d,std::string legend="No description",std::string group="default",bool modifiable=true)
{
  app->getApplicationInfoSpace()->fireItemAvailable(name,d);
  declareParameter(name,legend,group,modifiable);
}
private:
  std::map<std::string,std::string> legend_;
  std::map<std::string,std::string> group_;
  std::map<std::string,std::string> modifiable_;
  xdaq::ApplicationStub* app_;
std::set<xdaq::ApplicationDescriptor*> merged_;
};



#endif




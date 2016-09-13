/*

   FileName : 		FileServer.cc

   Content : 		FileServer module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 4.0

   Date of last modification : 19/12/2006

   Support : 		mail to : laurent.gross@ires.in2p3.fr
   
   Online help : 	https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem
*/
/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or*
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2005, Laurent GROSS - IReS/IN2P3
*/


#include "FileServer.h"


XDAQ_INSTANTIATOR_IMPL(FileServer)

	FileServer::FileServer(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
        {
		internalState_ = 0;
		

            /* Give funny and useless informations at load time */
            std::stringstream mmesg;
            mmesg << "Process version " << FILESERVER_PACKAGE_VERSION << " loaded";
            this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
            LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
            this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);


            /* bind xgi and xoap commands specific to this application */
            xgi::bind(this,&FileServer::Default, "Default");
            xgi::bind(this,&FileServer::Default1, "Default1");
            xgi::bind(this,&FileServer::callParseDiskPage, "callParseDiskPage");

	//CallBack to inform LogReader process of the logFileName currently in use
            xoap::bind(this,&FileServer::getCurrentLogFileName, "getCurrentLogFileName", XDAQ_NS_URI);


        }





	FileServer::~FileServer()
	{
	}




        /* xgi method called when the link <default_page> is clicked */
        void FileServer::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 0;
            Default(in, out);
        }



        /* xgi method called when the link <display_diagsystem> is clicked */
        void FileServer::callParseDiskPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	
		//A repository link has been clicked
		//Change current repository_ value so the newq rep content will be displayed
		//when Default() calls getRepositoryContentPage(in, out, currentRepository_);
	
	
		std::string requestedRepository = extractParameterValueFromURL(in, "REPOSITORY=");
		std::cout << "Request to move to repository : " << requestedRepository << std::endl;

		std::string requestedFile = extractParameterValueFromURL(in, "FILENAME=");
		std::cout << "Request to process file : " << requestedFile << std::endl;


		//IF a repository change is detected (VOID operation onb file), then change repository
		if (requestedFile == "VOID") currentRepository_= requestedRepository;

			
            internalState_ = 6;
            Default(in, out);
        }











        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void FileServer::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            *out << cgicc::p() << std::endl;
	    *out << "<br>Here comes the content of the Default page<br>" << std::endl;
            *out << cgicc::p() << std::endl;
        }




/* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
void FileServer::getRepositoryContentPage(xgi::Input * in, xgi::Output * out, std::string repositoryToParse ) throw (xgi::exception::Exception)
{






	//Get repository content
	getDiskContent(repositoryToParse);

	//Now build page content
	displayDiskContent(in, out);

}









        /* Displays the available HyperDaq links for this process */
        void FileServer::displayLinks(xgi::Input * in, xgi::Output * out)
        {
            /* Display default page */
            std::string urlBase_ = "/";
            urlBase_ += getApplicationDescriptor()->getURN();
            urlBase_ += "/Default1";
            *out << "<a href=" << urlBase_ << ">Default Page</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            /* Display diagnostic page */
            std::string urlParseFiles_ = "/";
            urlParseFiles_ += getApplicationDescriptor()->getURN();
            urlParseFiles_ += "/callParseDiskPage";
            *out << "<a href=" << urlParseFiles_ << ">Files Navigator Page</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            *out << "<br>";
            *out << cgicc::p() << std::endl;
        }






        /* Default() hyperDaq method */
        void FileServer::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

            /* Create HTML header */
            *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
            *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
            *out << cgicc::title("Diag File Server") << std::endl;
            xgi::Utils::getPageHeader(  out, 
                                        "Diag File Server", 
                                        getApplicationDescriptor()->getContextDescriptor()->getURL(),
                                        getApplicationDescriptor()->getURN(),
                                        "/daq/xgi/images/Application.jpg" );


            /* Display available links on top of the HTML page */
            displayLinks(in, out);
	    

            /* Get page HTML content according to the selected link */
            if (internalState_ == 0) getDefault1Content(in, out);
            if (internalState_ == 6)
	    {
	    	getRepositoryContentPage(in, out, currentRepository_);
	    }


            /* Create HTML footer */
            xgi::Utils::getPageFooter(*out);
        }



		

xoap::MessageReference FileServer::getCurrentLogFileName(xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	// Prepare reply body	
    xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName( "getLogFileNameReply", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement ( responseName );


	/* Add requested attributes to SOAP part */
	xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
	std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

	xoap::SOAPName logFileName ("logFileName", "", "");
	logElement[0].addAttribute(logFileName,"Any string as file name here");
	return reply;

}






/* Displays the available HyperDaq links for this process */
void FileServer::getDiskContent(std::string activeRepository)
{
	    
	//Make a system call to dump the working repository in temporary file
	if (activeRepository == "")
	{
		system("/bin/pwd > /tmp/repName.txt");
		   
		//Read back the working repository from file	    
		std::ifstream repfile;
		std::string repName;

		repfile.open("/tmp/repName.txt");
    		while (! repfile.eof() )
        	{
    			std::getline (repfile,repName);
	      		if (repName != "") currentRepository_ = repName;
        	}
        	repfile.close();
		if (currentRepository_ == "") currentRepository_ = "/tmp";
	}
	else currentRepository_ = activeRepository;




	//Fill in previous repository variable
	std::string::size_type lastSlashPos = currentRepository_.find_last_of("/");
	previousRepository_ = currentRepository_.substr(0,lastSlashPos);
	if (currentRepository_ == "/") previousRepository_ = "/";




	//Make a system call to dump the working repository in temporary file
	std::string sysCommand = "/bin/ls -F -1 " + currentRepository_ + " > /tmp/filesList.txt";
	system(sysCommand.c_str());
	   
	//Read back the working repository from file	    
	std::ifstream flistfile;
	std::string oneFileName;

	flistfile.open("/tmp/filesList.txt");
    	while (! flistfile.eof() )
        {
    		std::getline (flistfile,oneFileName);
	      if (oneFileName != "")
	      {
	      //Si la ligne lue finit par '/', c'est un répertoire. Test fait sur la condition : Si il y a au moins un '/' dans la ligne lue
	      std::string::size_type lastCharPos = std::string::npos;

	      lastCharPos = oneFileName.find_last_of("/");
	      	if (lastCharPos != std::string::npos)
	      	{
			oneFileName.erase(lastCharPos);
			repositoriesList_.push_back(oneFileName);
		}
		else
		{
			
			lastCharPos = oneFileName.find_last_of("*");
			if (lastCharPos != std::string::npos)
			{
				oneFileName.erase(lastCharPos);
				filesList_.push_back(oneFileName);
			}
			else
			{
				lastCharPos = oneFileName.find_last_of("@");
				if (lastCharPos != std::string::npos)
				{
					oneFileName.erase(lastCharPos);
					filesList_.push_back(oneFileName);
				}
				else
				{
					lastCharPos = oneFileName.find_last_of("=");
					if (lastCharPos != std::string::npos)
					{
						oneFileName.erase(lastCharPos);
						filesList_.push_back(oneFileName);
					}
					else filesList_.push_back(oneFileName);
				}
			}
			
				
		}
	    }
        }
        flistfile.close();

}







/* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
void FileServer::displayDiskContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	    
	*out << cgicc::p() << std::endl;


	std::string urlRepository_ = "/";
	urlRepository_ += getApplicationDescriptor()->getURN();
	urlRepository_ += "/callParseDiskPage";




	//Build current rep URL
	*out << "Active Repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
	*out << "?REPOSITORY=" << currentRepository_;
	*out << "&FILENAME=VOID";
	*out << ">" << currentRepository_ << "</a>" << "<br>" << std::endl;

	 *out << "<br>" << std::endl;



	//Build back URL
	if (currentRepository_ != "/")
	{		
		*out << "Top level repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
		*out << "?REPOSITORY=" << previousRepository_;
		*out << "&FILENAME=VOID";
		*out << ">" << previousRepository_ << "</a>" << "<br>" << std::endl;		
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Top level repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;Already at topmost level!<br>";
		*out << "<br>" << std::endl;
	}
 
 
 
 
	//Build repositories list
	if (repositoriesList_.size() > 0)
	{
		*out << "Local Repositories list : <br>";
		while (repositoriesList_.empty() == false)
		{
			*out << "&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
			*out << "?REPOSITORY=" << currentRepository_ << "/" << *(repositoriesList_.end() - 1);
			*out << "&FILENAME=VOID";
			 *out << ">" << *(repositoriesList_.end() - 1) << "/" << "</a>" << "<br>" << std::endl;
			repositoriesList_.pop_back();

		}
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Local Repositories list : <br>&nbsp;&nbsp;&nbsp;&nbsp;No sub-repositories detected<br>";
		*out << "<br>" << std::endl;
	}
	    
	 


	//Build files list
	if (filesList_.size() > 0)
	{
		*out << "Files list : <br>";
		while (filesList_.empty() == false)
		{
			*out << "&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
			*out << "?REPOSITORY=" << currentRepository_;
			*out << "&FILENAME=" << currentRepository_ << "/" << *(filesList_.end() - 1);
			*out << ">" << *(filesList_.end() - 1) << "</a>" << "<br>" << std::endl;
			filesList_.pop_back();

		}
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Files list : <br>&nbsp;&nbsp;&nbsp;&nbsp;No files detected in this repository<br>";
		*out << "<br>" << std::endl;
	}

	
	    
	*out << cgicc::p() << std::endl;
}








std::string FileServer::extractParameterValueFromURL(xgi::Input * in, std::string parameter)
{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
		//std::cout << "Querying parameter : " << parameter << " in URL : " << queryString << std::endl;

		std::string::size_type loc_begin = queryString.find( parameter, 0 );

		if (loc_begin != std::string::npos)
		{
			//std::cout << "Occurence found at string index : " << loc_begin << std::endl;

			char param_value[1000];
			int counter=0;
			int i=(loc_begin+strlen(parameter.c_str()));
			while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<990) )
			{
				param_value[counter] = (queryString.c_str())[i];
				counter++;
				i++;
			}
			param_value[counter] = '\0';
			return (std::string)param_value;
		}
		else
		{
			//std::cout << "Occurence " << parameter <<" not found in URL." << std::endl;
			return (std::string)"";
		}
}



















































Automatically monitor Q&A activity on a Stack Exchange site.                    

Command line options:

  -a, --all         Display all matching questions, not just updates            
  -p, --pagesize    Maximum number of questions to monitor at a time            
  -r, --rate        Rate at which to check for new activity, in minutes.        
                    This cannot be set less than 1 minute.                      
  -s, --site        Site to monitor                                             
  -t, --tagged      Tag(s) to monitor                                           
  -v, --version     Display version information                                 
      --help        Display usage information                                   
                                                                                
Examples:                                                                       

  `stack-watch -s scifi -t asoiaf`      Display updates under the "asoiaf tag" on the scifi site.                        
  `stack-watch -a -t http`              Display most recent "http" questions on the default site.                         
                    

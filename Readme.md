`stack-watch` is a Unix command-line utility for monitoring activity on the [Stack Exchange](http://stackexchange.com/) network of sites.

Installation
------------

`stack-watch` requires [libcurl](http://curl.haxx.se/libcurl/) and [json-c](https://github.com/json-c/json-c). These libraries can be installed using the following commands on a Debian-based Linux distribution (such as Ubuntu):

    sudo apt-get install libjson0 libjson0-dev
    sudo apt-get install libcurl4-openssl-dev

Once those dependencies are in place, `stack-watch` can be built from source:

    make
    ./stack-watch

Command line options
--------------------

    -a, --all         Display all matching questions, not just updates            
    -p, --pagesize    Maximum number of questions to monitor at a time            
    -r, --rate        Rate at which to check for new activity, in minutes.        
                      This cannot be set less than 1 minute.                      
    -s, --site        Site to monitor                                              
    -t, --tagged      Tag(s) to monitor                                           
    -v, --version     Display version information                                 
        --help        Display usage information                                   

Examples
--------

Display updates under the "asoiaf tag" on the scifi site:

    stack-watch -s scifi -t asoiaf
    
Display most recent "http" questions on the default site:

    stack-watch -a -t http
                    

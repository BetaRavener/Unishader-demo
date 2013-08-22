                                                    Unishader-demo
                                    Demo applications based on Unishader project
                                                 c) Ivan Sevcik, 2013
                                   zlib/libpng license  (see LICENSE for details)

### Installing

####Dependencies:
 - GLEW (http://glew.sourceforge.net/)
   GLEW is a cross-platform extension loading library.
 - SFML (http://www.sfml-dev.org)
   SFML is a cross-platform multimedia library.
 - UniShader (http://unishader.g6.cz/)
   UniShader is a cross-platform library for operating shader programs.

####Build system:
 - cmake (http://cmake.org/)
   Cmake is a cross-platform project generator.

####Installation steps:
    1. Create folder for demo (further refered as FOLDER).
    
    2. Use Cmake to generate project file. 
     In case Cmake can't find GLEW, SFML or UniShader,
     specify the paths to them manually (you will be prompted).
     (Linux) Navigate to FOLDER and use cmake -i ./ to configure make file.
     
    3. Build the generated project with Release configuration.
    
    4. Copy necessary dynamic libraries to FOLDER/bin/Release.
     List of them may vary according to your Cmake settings but you will
     be prompted for them upon executing generated binary.
     They are usually located at bin or lib folders of external libraries.
     (Linux) If all libraries are installed correctly you can skip this step.
     
    5. Run the demo application of your choice. At start you will be prompted
     in console window to enter paths to data and shaders folders. These are
     by default in folders of demo applications e.g. FOLDER/waves/data.
     Don't forget to add folder separator sign at the end as warned in console,
     otherwise the path will be incorrect.

### Usage

Refer readme files in FOLDER/doc for details and specific usage of each demo
application.  

Website: http://unishader.g6.cz/

TOP-SURF is an image descriptor that combines interest points with visual words, resulting in a high performance
yet compact descriptor that is designed with a wide range of content-based image retrieval applications in mind.
To support arbitrary image collections the descriptor uses a visual word dictionary that is created from 33.5
million pre-clustered interest points that were obtained from a large and diverse set of images. TOP-SURF offers
the flexibility to significantly reduce descriptor size and supports very fast image matching, making it useful
for diverse applications. In addition to the source code for the visual word extraction and comparisons, we also
provide a high level API and very large pre-computed codebooks (from 10,000 to 500,000 visual words) targeting web
image content for both research and teaching purposes.

AUTHORS

Bart Thomee	(bthomee@liacs.nl)
Erwin M. Bakker	(erwin@liacs.nl)
Michael S. Lew	(mlew@liacs.nl)

WEBSITE

http://press.liacs.nl/researchdownloads/topsurf

--------------------------------------------------

DOWNLOADING THE COMPILER

This has been tested on Apple OS X 10.6, and Ubuntu 10.04.

- If you don't yet have a version of the Eclipse C/C++ programming environment,
  Please go to this URL: http://www.eclipse.org/downloads
  
- Find the 'Eclipse IDE for C/C++ Developers' package, and select the one suitable for your platform.
  On the next page start the download by selecting the mirror closest to you.
  
- Unpack the archive to a directory of your choice.

- In that directory, double-click the 'Eclipse' icon to launch the programming environment.

- Once the compiler is running, please proceed with the steps below.


COMPILING THE TOP-SURF LIBRARY

- First of all, Eclipse will ask you for a workspace directory. Select a directory of your choice, or accept the
  default location offered to you by Eclipse.

- Go to 'File -> New -> C++ Project'.

- In the window that appears, fill in 'TOP-SURF' as the project name, keep the checkbox 'Use default location'
  ticked, and as project type choose 'Shared Library -> Empty Project'. Click on 'Finish'.

- You now have an empty project, so we need to copy the source files into it. First, unzip the source code archive
  topsurf_src_eclipse.zip to a directory of your choice. Then, in Eclipse, right-click on the project name 'TOP-SURF'
  and select 'Import'. In the new window, choose 'General -> File System' and click 'Next'. In the following screen,
  click the 'Browse' button and choose the directory into which you unzipped the source files. Ensure the checkbox
  is ticked before the name of the directory. Leave all other options as they are, and click 'Finish'.

- Finally, go to the menu 'Project' and select 'Build All' to compile the library.


ADVANCED COMPILATION SETTINGS

- By default, Eclipse will give the dynamic library it builds a default name, and only compiles it for the platform
  (32-bit or 64-bit) you are currently using. If you'd like to build for both platforms and/or want to create the
  library naming system we have used, we will provide you with instructions below. Note that using our library naming
  system makes it easier to understand how to integrate the library into other projects, something we describe in more
  detail in the section below.
  
- To recreate the 32-bit and 64-bit project settings we have used in Eclipse, do the following.

- Select the TOP-SURF project in the left pane, and right-click to show the context menu, from which you will select
  'Properties'. This opens a new window. In the left pane of this window, expand the 'C/C++ Build' option and then
  click 'Settings'.

- Click on the button named 'Manage Configuration', which is located in the top-right of the window. Rename the 'Debug'
  configuration to 'Debug x32', and rename 'Release' to 'Release x32'. Then, create a new configuration by clicking
  the 'New' button and name it 'Debug x64'. Ensure that the field 'Copy settings from Existing Configuration' is set
  to 'Debug x32. Do the same for 'Release x64', copying the settings from the 'Release x32' configuration. Click 'OK'
  to return to the project settings window.
  
- We will now make slight modifications to each of the configuration linker and compiler settings.
  + First of all, ensure that the 'Debug x32' configuration is selected, by selecting it from the dropdown list at the
    top of the window.
  + Then, go to the 'Tool Settings' tab and select the 'C++ Linker' entry (on OS X this is called 'MacOS X C++ Linker').
  + The 'Command' line will display 'g++'. Change this line so it shows 'g++ -m32'.
  + Next, move to the 'C++ Compiler' entry (on OS X this is called 'GCC C++ Compiler', and do the same. Change the
    command so it shows 'g++ -m32'.
  + Finally, go to the 'C Compiler' (on OS X this is called 'GCC C Compiler') and change the command to display 'gcc -m32'.
  + Click 'Apply' to update the project with the new configuration settings.
  + Change the configuration to 'Release x32' and do the exact same as you did for the 'Debug x32' configuration, i.e.
    add '-m32' to the command lines of the C++ Linker, C++ Compiler, and the C Compiler.
  + Change the configuration to 'Debug x64'. This time, instead of specifying '-m32', now you should specify '-m64'.
  + Do the same for the 'Release x64' configuration as you did for the 'Debug x64' configuration.
  
- Finally, we will make slight modifications to each of the configuration library naming settings.
  + For each of the configurations, go to the 'Build Artifact' tab.
  + Change the 'Artifact Name' to the following:
    o For 'Debug x32'   call it 'topsurf_x32d'
    o For 'Release x32' call it 'topsurf_x32'
    o For 'Debug x64'   call it 'topsurf_x64d'
    o For 'Release x64' call it 'topsurf_x64'
  + You should leave the 'Artifact Extension' ('dylib' on Mac OS X, and 'so' on Ubuntu) as it is.
  + You should leave the 'Output Prefix' ('lib') as it is.
  
- If you encounter compilation/linking errors when building, please read the following instructions to solve the
  two most common problems.
  + If you get an error in 'api.o' regarding 'Relocation R_X86_64_32 agains .bss cannot be used when making a shared object',
    which as far as we known only occurs when you are building a 64-bit library, then you need to add '-fPIC' to each of the
    'Command' lines in the C++ Linker, C++ Compiler, and C Compiler settings for both the 'Debug x64' and the 'Release x64'
    configurations. The C++ Linker and C++ Compiler command lines will then look like 'g++ -m64 -fPIC', whereas the C Compiler
    command line will look like 'gcc -m64 -fPIC'.
  + As a result from applying the above fix you may see an error regarding 'libstd++' on Ubuntu. In this case, please install
    the 'g++-multilib' and 'gcc-multilib' packages. The installation of these packages fixes an incorrectly set soft link
    in your system to the appropriate libstc++ library that is needed during the 64-bit install.


INTEGRATING THE TOP-SURF DESCRIPTOR INTO OTHER PROJECTS

- To use the shared library by other projects, right-click on the project name in Eclipse and select 'Import'. In the new window,
  choose 'General -> File System' and click 'Next'. In the following screen, click the 'Browse' button and choose the directory
  where you unzipped the shared library archive (e.g. topsurf_dll_bin_eclipse). Ensure the checkbox is ticked before the name of
  the directory. Leave all other options as they are, and click 'Finish'.
  
- Then attach the generated shared library files to your project.
  To achieve this, select the project in the left pane, and right-click to show the context menu, from which you will select
  'Properties'. This opens a new window. In the left pane of this window, expand the 'C/C++ Build' option and then click 'Settings'.
  
- We will now make slight modifications to each of the configuration linker and compiler settings.
  Note that you may want to follow the advanced compilation instructions again to create 32-bit and 64-bit builds for your project.
  In the following we use the 32-bit build as an example, and the 64-bit build is modified similarly.
  + First of all, ensure that the 'Debug x32' configuration is selected, by selecting it from the dropdown list at the
    top of the window.
  + Then, go to the 'Tool Settings' tab and select the 'C++ Linker' entry (on OS X this is called 'MacOS X C++ Linker').
  + The 'Command' line will display 'g++'. Change this line so it shows 'g++ -m32'.
  + Next, move to the 'C++ Compiler' entry (on OS X this is called 'GCC C++ Compiler', and do the same. Change the
    command so it shows 'g++ -m32'.
  + Finally, go to the 'C Compiler' (on OS X this is called 'GCC C Compiler') and change the command to display 'gcc -m32'.
  + Then, go back to the 'C++ Linker' and select the 'Libraries' entry. At the 'Libraries (-l)' field, click the icon with the little
    green plus (located on the right side) and type 'topsurf_x32d'. Then move to the 'Library Search Path (-L) field, and again click
    the associated icon with the green plus. In the appearing window, click the 'Workspace' button, select the top-level directory
    of your project, and click the 'OK' buttons to accept.
  + Click 'Apply' to update the project with the new configuration settings.
  + Change the configuration to 'Release x32' and do the exact same as you did for the 'Debug x32' configuration, i.e.
    add '-m32' to the command lines of the C++ Linker, C++ Compiler, and the C Compiler, and this time change the library to
    'topsurf_x32'.
  + Change the configuration to 'Debug x64'. This time, instead of specifying '-m32', now you should specify '-m64'. Also, change the
    library to 'topsurf_x64d'.
  + Do the same for the 'Release x64' configuration as you did for the 'Debug x64' configuration, but this time specify 'topsurf_x64'
    as the library.

- The API can now be used to access the TOP-SURF functionality. Make sure to add '#include "topsurf/api.h"' in each code file from
  where you want to call the API functions. Read the comments in 'api.h' for more details on how to call these functions correctly.

- Finally, after successfully compiling your project, make sure to bundle the previously generated TOP-SURF .dylib (on Mac OS X) or
  .so (on Ubuntu) files with the application's executable, as otherwise at runtime you will receive a message that this file is missing.


USAGE

Please refer to the api.h for the input parameters for functions and what is returned by them. In addition, take
a look at the example projects on the TOP-SURF website to see the descriptor actually being used. Note that at
least one of the dictionaries should be downloaded for the TOP-SURF library before it can be used. Because the
dictionaries are so large we have provided them in separate zip-files.


CREATING AND USING A NEW DICTIONARY

- To create a dictionary, look at the description of the function 'TopSurf_CreateDictionary' in the 'api.h' file.

- The main idea is that you give the function the location of a directory that contains a lot of images. You
  additionally have to pass the function several parameters that give it more information about how to extract
  interest points from the images and convert them into the desired number of visual words.

- After the dictionary is created, call 'TopSurf_SaveDictionary' with the location where you want to save it.

- You can use the newly created dictionary immediately, and now you are able to extract visual words from new
  images using your own dictionary. To use your dictionary at a later point in time, initialize the TOP-SURF
  library as per usual, and then call 'TopSurf_LoadDictionary' while passing it the location where your new
  dictionary is stored.


TROUBLESHOOTING

- On Linux and Mac OS X the path to an image, dictionary, etc., may contain a tilde (~), which acts as a shortcut to your
  home directory. For instance, ~/images/eiffeltower.jpg may actually point at /Volumes/John/images/eiffeltower.jpg.
  Because not all standard file functions are able to deal with that, you will need to replace the tilde by where it points
  at, as otherwise you are likely to get errors such as 'file not found' etc. A simple tilde expansion function is provided
  in the TOP-SURF library called TildeExpandPath() in the config.h/config.cpp files, which you can call to do the expansion,
  or you can copy the function into your own code if you prefer.
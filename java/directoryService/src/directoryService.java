//
//  directoryService.java
//
//  This needs to be compiled & signed in order to work properly;
//  we use the AccessController.doPriveleged() method call to break out of
//  the sandbox and return a list of files or directories within a given
//  path. The intended use is as a service for a web application that will
//  use javascript to invoke the method call and display the results.
//

import java.applet.*;
import java.security.*;
import java.lang.*;
import java.util.*;
import java.io.*;

public class directoryService extends Applet {
	
    public void init() {
    }
	
	//To be called from javascript
	public String[] getFiles(final String path)
	{
		return getItems(path, true, false);
	}

	//To be called from javascript
	public String[] getDirectories(final String path)
	{
		return getItems(path, false, true);
	}

	//May be called from javascript
	public String[] getItems(final String path, final boolean includeFiles, final boolean includeDirectories)
	{
		String[] contents = null;
		try {
			contents = (String[]) AccessController.doPrivileged(
				new PrivilegedAction() {
					public Object run() {
						return getContents(path, includeFiles, includeDirectories);
					}
				}
			);
        } catch (Exception e) {
        	e.printStackTrace();
			return new String[] {"An unexpected exception occurred"}; 
		}
		
		return contents;
	}
	
	/**
	 * This is the method that does the real work. This must be called from a priveleged context
	 * or it will throw an exception. Returns an array of strings, which are the fully-qualified
	 * path names for children of the input path.
	 */
	private String[] getContents(String path, boolean includeFiles, boolean includeDirectories)
	{
		ArrayList contents = new ArrayList();
		
		FileFilter filter = getFileFilter(includeFiles, includeDirectories);
		File dir = new File(path);

		File[] children = dir.listFiles(filter);
		if (children == null) {
			// You may want to change this to a constant of some sort, or just return null
			// and do the error checking in javascript.
			contents.add("[path not found]");
		} else {
			for(int i=0; i < children.length; i++)
			{
				contents.add(children[i].toString());
			}
		}
		return (String[]) contents.toArray(new String[contents.size()]);
	}
	
	/**
	 * Constructs and returns a filter suitable for including only files, only directories, both, or
	 * neither, as specified by the input parameters
	 */
	public FileFilter getFileFilter(final boolean includeFiles, final boolean includeDirectories)
	{
		return new FileFilter() {
			public boolean accept(File file) {
				return (file.isDirectory() && includeDirectories) || (!file.isDirectory() && includeFiles);
			}
		};
	}
}

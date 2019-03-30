/*
 * Fireball for Java 8
 * Copyright (C) 2017 Anthony Pulse, Jr. @thexiv inland14@live.com
 *
 * This is not to be modified, shared, or copied without written
 * permission of the author. Any use otherwise is lawful.
 *
 */
package phpson;

import java.util.Vector;
import java.util.*;
import java.util.HashMap;

public class MapMaker {

	public static Vector<String> vs = new Vector<String>();
	
    public static String recurMKey (HashMap<String, HashMap<String,String>> input) {
    	String output = "{";
    	Set<String> keys = input.keySet();
    	for (String p : keys) {
    		if (input.containsKey(p)) {
    			HashMap<String, String> v = input.get(p);
    			output += "'" + p + "':{";
    			output += recurMValue(v) + "},";
    		}
    	}
    	output = output.substring(0, output.length()-1);
    	System.out.println(output);
    	return output;
    }
    
    public static String recurMValue (HashMap<String,String> input) {
    	String output = "";
    	Set<String> keys = input.keySet();
    	for (String p : keys) {
    		if (input.containsKey(p)) {
    			String v = input.get(p); 
    			output += '\'' + p + "':'" + v + "',";
    		}
    	}
    	output = output.substring(0, output.length()-1);
    	return output;
    }
/*
    public static Vector<String> retVect(byte[] b) {
    	String vls = "";
		char bn = ' ';
		//char[] bx = b.toCharArray();
		for (byte l : b) {
			if ((char) l == '{' || (char) l == '[') {
				vs.add((char)l);
				vls = "";
				continue;
			}
			else if ((char) l == '}' || (char) l == ']') {
				vs.add((char) l);
				bn = (char) l;
				
				continue;
			}
			else if ((bn == '}' || bn == ']') && (char) l == ',') {
				vs.elementAt(vs.size()-1) = bn + (char) l;
				bn = ' ';
				continue;
			}
			else if ((char) l == '}' || (char) l == ']') {
				vs.add((char) l);
				vls = "";
				continue;
			}
			else if (vls.length() > 1 && vls.startsWith((char) l) && ((char) l == '\'' || (char) l == '"')) {
				vs.add(vls + (char) l);
				System.out.print("\n.");
				vls = "";
				continue;
			}
			vls += (char) l;
		}
		return vs;
    }
    public static Vector<String> retVect(String b) {
    	String vls = "";
		char[] bx = b.toCharArray();
		for (String l : bx) {
			if (l.endsWith("{") || l.endsWith("[")) {
				vs.add(l);
				vls = "";
				continue;
			}
			if (l.endsWith("}") || l.endsWith("]")) {
				vs.add(l);
				vls = "";
				continue;
			}
			else if (vls.length() > 1 && vls.startsWith(l) && (l.endsWith("'") || l.endsWith("\""))) {
				vs.add(vls + l);
				vls = "";
				continue;
			}
			vls += l;
		}
		return vs;
    }
  */  
    public static HashMap<String, HashMap<String, String>> Maker (Vector<String> varl) {
        HashMap<String, HashMap<String,String>> mapping = new HashMap<String, HashMap<String, String>>();
        HashMap<String, String> values = new HashMap<>();
        Vector<String> depth = new Vector<>();
        String title = "";
        Integer x = 0, i = 0;
        for (String arl : varl) {
            if ((arl.startsWith("'") || arl.startsWith("\"")) && arl.endsWith(":")) {
                title = arl.substring(1,arl.length()-2).replace("%20"," ");
                depth.add(title);
                x++;
            }
            if (x > 0 && (arl.startsWith("{") || arl.startsWith("["))) {
                i++;
                String val = arl + i.toString();
                x++;
                if (depth.size() > 0)
                	values.put(depth.lastElement(), val);
                title = "";
            }
            if ((arl.startsWith("\"") && arl.endsWith("\"")) || (arl.startsWith("'") && arl.endsWith("'"))) {
            	
                String val = arl;
                if (val.length() > 2) {
                	val = val.substring(1,arl.length()-1);
                    val = val.replace("%20"," ");
                }
                else
                	val = null;
                x++;
                if (depth.size() > 0) {
                	values.put(depth.lastElement(),val);
                	depth.remove(depth.lastElement());
                }
                title = "";
            }
            if (arl.matches("[0-9_]+")) {
                String val = arl;
                x++;
                values.put(title,val);
                depth.remove(depth.lastElement());
                title = "";
            }
            while (x < varl.size() && (arl.startsWith("}") || arl.startsWith("]"))) {
                String end = "END" + i.toString() + "-"  + x.toString();
                i--;
                if (depth.size() > 0) {
                	mapping.put(depth.lastElement(), values);
                	depth.remove(depth.lastElement());
                }
                values.put(end, arl);
                x++;
            }
        }
        return mapping;
    }
}

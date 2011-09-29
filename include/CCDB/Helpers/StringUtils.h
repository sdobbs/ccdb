﻿#ifndef _StringUtils_
#define _StringUtils_

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#endif

//#include <stdargs.h>
//#include <varargs.h>
#include <stdio.h>

#include <stdarg.h>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <exception>

#include <iosfwd>
#include <iostream>
#include <ostream>
#include <time.h>


#include <CCDB/Helpers/Varargs.h>

#define CCDB_BLANK_CHARACTERS " \n\t\v\r\f"
//checks if character is blank.
//returns true if char is one of CCDB_BLANK_CHARACTERS
#define CCDB_CHECK_CHAR_IS_BLANK(character) ((character)==' ' || (character)=='\n' || (character)=='\t' || (character)=='\v' || (character)=='\r' || (character)=='\f')
using namespace std;



namespace ccdb
{

#define array_length(array) (sizeof (array) / sizeof (array)[0])


struct DAssignmentRequest
{
	std::string Path;
	        int RunNumber;
	std::string Variation;
	     time_t Time;
};


class StringUtils
{
public:

    /**
	 * @brief printf like string formatter
	 * 
	 * printf like string formatter, based on @see vsprintf
	 * The reason to implement this function is that 
	 * except of sprintf and snprintf the string length 
	 * is determined automathically
	 * @param fmt
	 * @return 
	 */
    static std::string Format(const char *va_(fmt), ...);
	
	
	//static string	Format(const char *fmt, ...);			
	
	/**
	 * @brief vprintf like string formatter
	 * 
	 * vprintf like string formatter, based on @see vsprintf
	 * The reason to implement this function is that 
	 * except of sprintf and snprintf the string length 
	 * is determined automathically
	 * @param fmt
	 * @param ap
	 * @return 
	 */
	static string	vFormat(const char *fmt, va_list ap);	

	/** @brief Encodes string to add to DB
	 * Encode
	 *
	 * @param     const string & source
	 * @return   std::string
	 */
	static string	Encode(const string& source);

	/** @brief Decode string from encoded string
	 *
	 * @param     const string & source
	 * @return   std::string
	 */
	static string	Decode(const string& source);
	
	/** @brief string Replace
	 *
	 * @param     const string & pattern
	 * @param     const string & replace
	 * @param     const string & source
	 * @param     string & out
	 * @return   int
	 */
	static int		Replace(const string& pattern, const string& replace, const string& source, string &out);

	/** @brief String Replace
	 *
	 * @param     const string & pattern
	 * @param     const string & replace
	 * @param     const string & source
	 * @return   std::string
	 */
	static string	Replace(const string& pattern, const string& replace, const string& source);

	/** @brief Extracts Directory from object path
	 *
	 *	in /dir/subdir/object will return /dir/subdir
	 *
	 * @param     const string & path
	 * @return   std::string
	 */
	static string	ExtractDirectory( const string& path );

	/** @brief Extract Objectname from object path
	 *
	 * @param     const string & path
	 * @return   std::string
	 */
	static string	ExtractObjectname( const string& path );

	/** @brief Combines two parts of path
	 *
	 * will hande /path/sub/ + /path/right etc.
	 *
	 * @param     const string & left
	 * @param     const string & right
	 * @return   std::string
	 */
	static string	CombinePath( const string& left, const string& right );

	/** @brief search pattern in source using * and ?
	 *
	 * @param pattern
	 * @param source
	 * @return   bool Returns:true on match, false on no match.
	 */
	static bool WildCardCheck(const char* pattern, const char* source);

	
	/** @brief
	 * Split
	 *
	 * @param     const string & str
	 * @param     vector<string> & tokens
	 * @param     const string & delimiters
	 * @return   std::vector<std::string> &
	 */
	static vector<string> &Split(const string& str, vector<string>& tokens, const string& delimiters = " ");


	/** @brief
	 * Split
	 *
	 * @param     const std::string & s
	 * @param     char delim
	 * @return   std::vector<std::string>
	 */
	static std::vector<std::string> Split(const std::string &s, const string& delimiters = " ");

    
	
	/**
	 * @brief trims string from the both sides
	 * 
	 * @warning the method changes the string
	 * removes blank characters:
	 * " ", "\n", "\t", "\v", "\r", "\f"
	 * from the beginning and the end of the string
	 * 
	 * @param [in,out] s - string that will be trimmed
	 */
	static  void Trim( string& s )
	{	
		// Remove leading and trailing whitespace
		static const char whitespace[] = CCDB_BLANK_CHARACTERS;
		s.erase( 0, s.find_first_not_of(whitespace) );
		s.erase( s.find_last_not_of(whitespace) + 1U );
	}
	
    /** Splits string to lexical values.
     *
     * LexicalSplit treats:
     * 1) "quoted values" as one value,
     * 2) '#' not in the beginning of the file are treated as comments to the end of the line
     * 3) skips all white space characters. All specification is in doc/ccdb_file_format.pdf
     * 
     * @remark
     * Handling inconsistencies and errors while readout parse time:
     *  ●  No ending quote . If no ending “ is found, string value will be taken
     *     until the end of line.
     *  ●  Comment inside a string. Comment symbol inside the line is ignored. 
     *     So if you have a record in the file “info #4” it will be read just
     *     as “info #4” string
     *  ●  Sticked string. In case of there is no spaces between symbols and
     *     an quotes, all will be merged as one string. I.e.:
     *     John" Smith" will be parsed as one value: "John Smith"
     *     John" "Smith will be parsed as one value: "John Smith"
     *     but be careful(!) not to forget to do a spaces between columns
     *     5.14”Smith” will be parsed as one value “5.14Smith” that probably would
     *     lead to errors if these were two different columns
     *  ●  If data contains string fields they are taken into “...” characters. All “
     *     inside string should be saved by \” symbol. All words and symbols
     *     inside “...” will be interpreted as string entity.
     *
     */
    static std::vector<string> LexicalSplit(const std::string& source);
	
      /** Splits string to lexical values.
     *
     * LexicalSplit treats:
     * 1) "quoted values" as one value,
     * 2) '#' not in the beginning of the file are treated as comments to the end of the line
     * 3) skips all white space characters. All specification is in doc/ccdb_file_format.pdf
     * 
     * @remark
     * Handling inconsistencies and errors while readout parse time:
     *  ●  No ending quote . If no ending “ is found, string value will be taken
     *     until the end of line.
     *  ●  Comment inside a string. Comment symbol inside the line is ignored. 
     *     So if you have a record in the file “info #4” it will be read just
     *     as “info #4” string
     *  ●  Sticked string. In case of there is no spaces between symbols and
     *     an quotes, all will be merged as one string. I.e.:
     *     John" Smith" will be parsed as one value: "John Smith"
     *     John" "Smith will be parsed as one value: "John Smith"
     *     but be careful(!) not to forget to do a spaces between columns
     *     5.14”Smith” will be parsed as one value “5.14Smith” that probably would
     *     lead to errors if these were two different columns
     *  ●  If data contains string fields they are taken into “...” characters. All “
     *     inside string should be saved by \” symbol. All words and symbols
     *     inside “...” will be interpreted as string entity.
     *
     */
	static void LexicalSplit(std::vector<string>& tokens, const std::string& source);

	/** @brief Prints object to string
	 *
	 * @param     obj Object to be converted to string. Should have << stream 
	 * @return   String with value, if object doesn't have reloaded operator <<, return string::empty()
	 */
	template<class T>
	static std::string ToString(const T& obj)
	{
		std::stringstream sstream (stringstream::out);
		string result(string::empty());

		try
		{
			sstream << obj;
			result = sstream.str();
		}
		catch (std::exception ex) {	/*eat it*/}
		return result;
	}
	
	static int				ParseInt(const string& source, bool *result=NULL );		    ///Reads int	from the last query row
	static unsigned int		ParseUInt(const string& source, bool *result=NULL );        ///Reads unsigned int from the last query row
	static long				ParseLong(const string& source, bool *result=NULL );        ///Reads long from the last query row
	static unsigned long	ParseULong(const string& source, bool *result=NULL );       ///Reads unsigned long from the last query row
	static bool				ParseBool(const string& source, bool *result=NULL );        ///Reads bool from the last query row
	static double			ParseDouble(const string& source, bool *result=NULL );      ///Reads double from the last query row
	static string			ParseString(const string& source, bool *result=NULL );      ///Reads string from the last query row
	static time_t			ParseUnixTime(const string& source, bool *result=NULL );    ///Reads string from the last query row
};


}
#endif // StringUtils_h__


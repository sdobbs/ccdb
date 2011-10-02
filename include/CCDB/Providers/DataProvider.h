#ifndef _DDataProvider_
#define _DDataProvider_

#include <string>
#include <vector>
#include <map>

#include "CCDB/Model/ObjectsOwner.h"
#include "CCDB/Model/Assignment.h"
#include "CCDB/Model/ConstantsTypeTable.h"
#include "CCDB/Model/Directory.h"
#include "CCDB/Model/RunRange.h"
#include "CCDB/Model/Variation.h"
#include "CCDB/CCDBError.h"



/* @class DDataProvider
 * This is the main base interface to the Providers class family. Providers - are classes 
 * derived from this class, each Provider provides data from/to a specified data source. 
 * I.e. MySQLDataProvider works with MySQL database.
 *
 *==============================================
 *Low Level API
 *
 *                    ^ ^ ^  
 *                    | | |                            
 *  +------------------------------------------+
 *  | Data Model: DAssignment, DTypeTable, ... |    -   Data model is returned to user
 *  +------------------------------------------+        
 *                       ^                              
 *                       |                              
 *                       |                              
 *  +------------------------------------------+        
 *  |   DDataProvider - Interface to database  |    -   User get data by using DDataProvider functions
 *  +------------------------------------------+        
 *                       |                              
 *             +---------------------+                  
 *            /                       \                 
 *  +----------------+        +----------------+        
 *  | DMySQLProvider |        | DSQLiteProvider|    -   These classes are inherited from DDataProvider and do actual querries to related sources
 *  +----------------+        +----------------+        
 *          |                          |                
 *  <================>        <================>        
 *  | MySQL Database |        |     SQLite     |    -   Data storages 
 *  <________________>        <________________>
 *
 *
 *
 *
 */
namespace ccdb
{

class DataProvider: public ObjectsOwner
{
public:

    DataProvider(void);
    virtual ~DataProvider(void);
    
    //----------------------------------------------------------------------------------------
    //	C O N N E C T I O N
    //----------------------------------------------------------------------------------------

    /**
     * @brief Connects to database using connection string
     * 
     * Connects to database using connection string
     * connection string might be in form: 
     * mysql://<username>:<password>@<mysql.address>:<port> <database>
     * 
     * @param connectionString "mysql://<username>:<password>@<mysql.address>:<port> <database>"
     * @return true if connected
     */
    virtual bool Connect(std::string connectionString) = 0;

    /**
     * @brief closes connection to data
     */
    virtual void Disconnect()= 0;

    /**
     * @brief indicates ether the connection is open or not
     * 
     * @return true if  connection is open
     */
    virtual bool IsConnected()=0;
    
    /** @brief Connection string that was used on last successful connect.
     *
     * Connection string that was used on last successful connect.
     * If no any successfull connects were done string::empty will be returned
     *
     * @return  Last connection string or string::empty() if no successfull connection was done
     */
    virtual std::string GetConnectionString();

    //----------------------------------------------------------------------------------------
    //	D I R E C T O R Y   M A N G E M E N T
    //----------------------------------------------------------------------------------------
    
    /** @brief Gets directory by its full path
    *
    * @param   Full path of the directory
    * @return DDirectory object if directory exists, NULL otherwise
    */
    virtual Directory* GetDirectory(const string& path)=0;

    /** @brief return reference to root directory
     * 
     * @warning User should not delete this object 
     *
     * @return   DDirectory object pointer
     */
    virtual Directory * const GetRootDirectory()=0;

    /** @brief Searches for directory 
     *
     * Searches directories that matches the pattern 
     * inside parent directory with path @see parentPath 
     * or globally through all type tables if parentPath is empty
     * The pattern might contain
     * '*' - any character sequence 
     * '?' - any single character
     *
     * paging could be done with @see startWith  @see take
     * startWith=0 and take=0 means select all records;
     *
     * objects that are contained in vector<DDirectory *>& resultDirectories will be
     * 1) if this provider owned - deleted (@see ReleaseOwnership)
     * 2) if not owned - just leaved on user control
     *
     * @param  [out] resultDirectories	search result
     * @param  [in]  searchPattern		Pattern to search
     * @param  [in]  parentPath			Parent path. If NULL search through all directories
     * @param  [in]  startRecord		record number to start with
     * @param  [in]  selectRecords		number of records to select. 0 means select all records
     * @return bool true if there were error (even if 0 directories found) 
     */
    virtual bool SearchDirectories(vector<Directory *>& resultDirectories, const string& searchPattern, const string& parentPath="", int take=0, int startWith=0)=0;
    
    
    /** @brief SearchDirectories
     *
     * Searches directories that matches the pattern 
     * inside parent directory with path @see parentPath 
     * or globally through all type tables if parentPath is empty
     * The pattern might contain
     * '*' - any character sequence 
     * '?' - any single character
     *
     * paging could be done with @see startWith  @see take
     * startWith=0 and take=0 means select all records;
     *
     * objects that are contained in vector<DDirectory *>& resultDirectories will be
     * 1) if this provider owned - deleted (@see ReleaseOwnership)
     * 2) if not owned - just leaved on user control
     * @param  [in]  searchPattern		Pattern to search
     * @param  [in]  parentPath			Parent path. If NULL search through all directories
     * @param  [in]  startRecord		record number to start with
     * @param  [in]  selectRecords		number of records to select. 0 means select all records
     * @return list of 
     */
    virtual vector<Directory *> SearchDirectories(const string& searchPattern, const string& parentPath="", int take=0, int startWith=0);


    /** @brief Creates directory using parent parentDirFullPath
     * 
     * @warning in current realization, if operation succeeded 
     * the directories structure will be rebuilded. This mean that 
     * (!) all previous pointers to DDirectories except Root Directory
     * will become deleted => unusable
     * 
     * @param newDirName Name of the directory. Contains A-Z, a-z,0-9, _, -, 
     * @param parentDirFullPath Path of the parent directory. 
     * @param comment comment on this dir. Might be NULL
     * @return 
     */
    virtual bool MakeDirectory(const string& newDirName, const string& parentDirFullPath, const string& comment = NULL)=0;

    /** @brief Updates directory
     *
     * @warning in current realization, if operation succeeded 
     * the directories structure will be rebuilded. This mean that 
     * (!) all previous pointers to DDirectory objects except Root Directory
     * will become deleted => unusable
     * 
     * @param  dir		Directory to update
     * @return   bool
     */
    virtual bool UpdateDirectory(Directory *dir)=0;

    /**
     * @brief Deletes directory using parent path
     *
     *	"/" - root directory can't be deleted
     *
     * @warning in current realization, if operation succeeded 
     * the directories structure will be rebuilded. This mean that 
     * (!) all previous pointers to DDirectories except Root Directory
     * will become deleted => unusable
     * 
     * @param  [in] path Path of the directory. 
     * @return true if no errors 
     */
    virtual bool DeleteDirectory(const string& fullPath)=0;

    /**
     * @brief Deletes directory using directory object
     *
     *	"/" cant be deleted
     *
     * @warning in current realization, if operation succeeded 
     * the directories structure will be rebuilded. This mean that 
     * (!) all previous pointers to DDirectories except Root Directory
     * will become deleted => unusable
     *
     * @param  [in] dir	Directory to Delete
     * @return true if no errors 
     */
    virtual bool DeleteDirectory(Directory *dir) =0;

    /** NEVER USE IT UNLESS YOU KNOW
     *  Deletes directory and all tables and subdirectories in it
     *
     *  @remarks This function is here to use ccdb as a general data storage only 
     *  this function is against the philosofy "No delete. Any change by adding New"
     *
     */
    virtual bool RecursiveDeleteDirectory(Directory *dir);


    //----------------------------------------------------------------------------------------
    //	C O N S T A N T   T Y P E   T A B L E
    //----------------------------------------------------------------------------------------

    
    /** @brief Gets ConstantsType information from the DB
     *
     * @param  [in] path absolute path of the type table
     * @return new object of DConstantsTypeTable
     */
    virtual ConstantsTypeTable * GetConstantsTypeTable(const string& path, bool loadColumns=false)=0;

    /** @brief Gets ConstantsType information from the DB
     *
     * @param  [in] name name of ConstantsTypeTable
     * @param  [in] parentDir directory that contains type table
     * @return new object of DConstantsTypeTable
     */
    virtual ConstantsTypeTable * GetConstantsTypeTable(const string& name, Directory *parentDir, bool loadColumns=false)=0;

    /** @brief Gets ConstantsType information from the DB
     *
     * @param  [in] name name of ConstantsTypeTable
     * @param  [in] parentDir directory that contains type table
     * @return new object of DConstantsTypeTable
     */
    virtual bool GetConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables,const string& parentDirPath, bool loadColumns=false) =0;

    /** @brief Get all "constants" from current directory
     *
     * @param parentDir
     * @return vector of constants
     */
    virtual vector<ConstantsTypeTable *> GetConstantsTypeTables (Directory *parentDir, bool loadColumns=false)=0;

    /** @brief Get all "constants" from current directory
     *
     * @param parentDir
     * @return vector of constants
     */
    virtual bool GetConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables, Directory *parentDir, bool loadColumns=false) =0;
    
    /** @brief Searches for type tables that matches the patten
     *
     * Searches type table that matches the pattern 
     * inside parent directory with path @see parentPath 
     * or globally through all type tables if parentPath is empty
     * The pattern might contain
     * '*' - any character sequence 
     * '?' - any single character
     *
     * paging could be done with  @see take ans @see startWith 
     * take=0, startWith=0 means select all records;
     *
     * objects that are contained in vector<DDirectory *>& resultDirectories will be
     * 1) if this provider owned - deleted (@see ReleaseOwnership)
     * 2) if not owned - just leaved on user control
     * @param  pattern
     * @param  parentPath
     * @return bool
     */
    virtual bool SearchConstantsTypeTables(vector<ConstantsTypeTable *>& typeTables, const string& pattern, const string& parentPath = "", bool loadColumns=false, int take=0, int startWith=0 )=0;

    /** @brief Searches for type tables that matches the patten
     *
     * Searches type table that matches the pattern 
     * inside parent directory with path @see parentPath 
     * or globally through all type tables if parentPath is empty
     * The pattern might contain
     * '*' - any character sequence 
     * '?' - any single character
     *
     * paging could be done with @see startWith  @see take
     * startRecord=0 and selectRecords=0 means select all records;
     *
     * objects that are contained in vector<DDirectory *>& resultDirectories will be
     * 1) if this provider owned - deleted (@see ReleaseOwnership)
     * 2) if not owned - just leaved on user control
     * @param  pattern
     * @param  parentPath
     * @return vector<DConstantsTypeTable *>
     */
    virtual vector<ConstantsTypeTable *> SearchConstantsTypeTables(const string& pattern, const string& parentPath = "", bool loadColumns=false, int take=0, int startWith=0 )=0;
    
    /**
     * @brief This function counts number of type tables for a given directory 
     * @param [in] directory to look tables in
     * @return number of tables to return
     */
    virtual int CountConstantsTypeTables(Directory *dir)=0;
    
    /** @brief Loads columns for this table
     *
     * @param parentDir
     * @return vector of constants
     */
    virtual bool LoadColumns(ConstantsTypeTable* table) =0;

    /** @brief Creates constant table in database
     *
     * @param  table
     * @return bool
     */
    virtual bool CreateConstantsTypeTable(ConstantsTypeTable *table)=0;
    
    /** @brief Creates constant table in database
     * 	
     * Creates  constants type table in database and returns reference to
     * created table if operation is succeeded (NULL otherwise)
     * 
     * The map "columns" should contains <"name", "type"> string pairs where:
     *  -- "name" is the name of the column.
     *      Should have the same naming convetions
     *      as names of directories and type tables  @see ValidateName() 
     *  -- "type" is the type of the column 
     *     might be: int, uint, long, ulong, double, bool, string 
     *     other values will lead to "double" type to be used.
     *     @see ccdb::ConstantsTypeColumn::StringToType
     *     Thus <"px", "">, <"py", ""> will create two double typed columns
     * 
     * @param [in] name			name of the new constants type table 
     * @param [in] parentPath   parent directory path
     * @param [in] rowsNumber  Number of rows
     * @param [in] columns     a map fo "name", "type" pairs
     * @param [in] comments		description for this type table
     * @return NULL if failed, pointer to created object otherwise
     */
    virtual ConstantsTypeTable* CreateConstantsTypeTable(const string& name, const string& parentPath, int rowsNumber, map<string, string> columns, const string& comments ="") =0;
    
    /** @brief Creates constant table in database
     * 	
     * Creates  constants type table in database and returns reference to
     * created table if operation is succeeded (NULL otherwise)
     * 
     * The map "columns" should contains <"name", "type"> string pairs where:
     *  -- "name" is the name of the column.
     *      Should have the same naming convetions
     *      as names of directories and type tables  @see ValidateName() 
     *  -- "type" is the type of the column 
     *     might be: int, uint, long, ulong, double, bool, string 
     *     other values will lead to "double" type to be used.
     *     @see ccdb::ConstantsTypeColumn::StringToType
     *     Thus <"px", "">, <"py", ""> will create two double typed columns
     * 
     * @param [in] name			name of the new constants type table 
     * @param [in] parentDir   parent directory
     * @param [in] rowsNumber  Number of rows
     * @param [in] columns     a map fo "name", "type" pairs
     * @param [in] comments		description for this type table
     * @return NULL if failed, pointer to created object otherwise
     */
    virtual ConstantsTypeTable* CreateConstantsTypeTable(const string& name, Directory *parentDir, int rowsNumber, map<string, string> columns, const string& comments ="")=0;
    
    /** @brief Uptades constant table in database
     *
     * Thes function updates only:
     * - name
     * - parent directory
     * - commens
     * The function doesnt change any columns and rows number
     * It assumed, that a user should delete the TypeTable and recreate it 
     * to perform such changes because it will break all data already stored
     * @param   [in] table with updated info
     * @return bool
     */
    virtual bool UpdateConstantsTypeTable(ConstantsTypeTable *table)=0;


    /** @brief Deletes constant type table
     * 
     * Deletes constant type table. 
     * The type table will not be deleted if any assignment for this table exists
     * Used should delete assignments first and only than delete the type table.
     * 
     * @param   table table info
     * @return bool true if success
     */
    virtual bool DeleteConstantsTypeTable(ConstantsTypeTable *table)=0;

    /** NEVER USE IT UNLESS YOU KNOW
     *  Deletes type table and all assignments in it
     *
     *  @remarks This function is here to use ccdb as a general data storage only 
     *  this function is against the philosofy "No delete. Any change by adding New"
     *
     */
    virtual bool RecursiveDeleteTypeTable(ConstantsTypeTable *dir);
    
    //----------------------------------------------------------------------------------------
    //	R U N   R A N G E S
    //----------------------------------------------------------------------------------------

    /** @brief Creates RunRange in db
     *
     * TODO remove method
     * @param     DRunRange * run
     * @return   bool
     */
    virtual bool CreateRunRange(RunRange *run)=0;

    /** @brief GetRun Range from db
     *
     * @param     int min
     * @param     int max
     * @param     const char * name
     * @return   DRunRange* return NULL if not found or failed
     */
    virtual RunRange* GetRunRange(int min, int max, const string& name = "")=0;

    
    /**
     * @brief Searches all run ranges associated with this type table
     * 
     * @param [out] resultRunRanges result run ranges
     * @param [in]  table		table to search run ranges in
     * @param [in]  variation	variation to search, if not set all variations will be selected
     * @param [in]  take			how many records to take
     * @param [in]  startWith	start record to take
     * @return 
     */
    virtual bool GetRunRanges(vector<RunRange *>& resultRunRanges, ConstantsTypeTable *table, const string& variation="", int take=0, int startWith=0 )=0;
    
    /**
     * @brief Searches all run ranges associated with this type table
     * 
     * @param [out] resultRunRanges result run ranges
     * @param [in]  table		table to search run ranges in
     * @param [in]  variation	variation to search, if not set all variations will be selected
     * @param [in]  take			how many records to take
     * @param [in]  startWith	start record to take
     * @return 
     */
    virtual bool GetRunRanges(vector<RunRange *>& resultRunRanges, const string& typeTablePath, const string& variation="",int take=0, int startWith=0 );
    
    /** @brief GetRun Range from db
     *
     * @param     int min
     * @param     int max
     * @param     const char * name
     * @return   DRunRange* return NULL if not found or failed
     */
    virtual RunRange* GetRunRange(const string& name)=0;

    /** @brief Gets run range from DB Or Creates RunRange in DB
     *
     * @param min run ranges limits
     * @param max run ranges limits
     * @param name name of the run range
     * @return   DRunRange* return NULL if failed
     */
    virtual RunRange* GetOrCreateRunRange(int min, int max, const string& name="", const string& comment="")=0;
    
    /** @brief Updates run range
     * 
     * Function updates:
     * - start run number
     * - end run number
     * - comment
     * does not update run range name and id
     * @param [in] run runrange model with proper id and updated data
     * @return bool if no errors and data is changed
     */
    virtual bool UpdateRunRange(RunRange* run)=0;
    
    /** @brief Deletes run range
     * @param [in] run model object to delete
     * @return true if no errors and run range deleted
     */
    virtual bool DeleteRunRange(RunRange* run)=0;

    /** NEVER USE IT UNLESS YOU KNOW
     *  Deletes run range and all assignments in it
     *
     *  @remarks This function is here to use ccdb as a general data storage only 
     *  this function is against the philosophy "No delete. Any change by adding New"
     *
     */
    virtual bool RecursiveDeleteRunRange(RunRange *dir);

    //----------------------------------------------------------------------------------------
    //	V A R I A T I O N
    //----------------------------------------------------------------------------------------
    /** @brief Get variation by name
     *
     * @param     const char * name
     * @return   DVariation*
     */
    virtual Variation* GetVariation(const string& name)=0;
     
    /**
     * @brief Searches all variations associated with this type table
     * @param  [out] resultVariations result variations
     * @param  [in]  table table to search run ranges in
     * @param  [in]  run   specified run to search for, if 0 searches for all run ranges
     * @param  [in]  take how many records to take
     * @param  [in]  startWith start record to take
     * @return 
     */
    virtual bool GetVariations(vector<Variation *>& resultVariations, ConstantsTypeTable *table, int run=0, int take=0, int startWith=0 )=0;

    /**
     * @brief Searches all variations associated with this type table
     * @param  [out] resultVariations result variations
     * @param  [in]  table table to search run ranges in
     * @param  [in]  run   specified run to search for, if 0 searches for all run ranges
     * @param  [in]  take how many records to take
     * @param  [in]  startWith start record to take
     * @return 
     */
    virtual vector<Variation *> GetVariations(ConstantsTypeTable *table, int run=0, int take=0, int startWith=0 )=0;
    
    /**
     * @brief Searches all variations associated with this type table
     * @param  [out] resultVariations result variations
     * @param  [in]  path path to table to search run ranges in
     * @param  [in]  run   specified run to search for, if 0 searches for all run ranges
     * @param  [in]  take how many records to take
     * @param  [in]  startWith start record to take
     * @return 
     */
    virtual bool GetVariations(vector<Variation *>& resultVariations, const string& path, int run=0, int take=0, int startWith=0 );
    
    /** @brief Create variation
     *
     * @param  [in] DVariation * variation
     * @return true if success
     */
    virtual bool CreateVariation(Variation *variation)=0;

    /** @brief Update variation
     *
     * Function updates:
     * variation comments
     * variation name if there is no variations with this name
     * 
     * @param   [in]  variation to update
     * @return true if success  
     */
    virtual bool UpdateVariation(Variation *variation) =0;

    /** @brief Delete variation
     *
     * Variation will be deleted only 
     * if there is no assignments for this variation
     * @param    [in] variation to delete
     * @return   bool
     */
    virtual bool DeleteVariation(Variation *variation) =0;

    /** NEVER USE IT UNLESS YOU KNOW
     *  Deletes variation and all assignments that belongs to it 
     *
     *  @remarks This function is here to use ccdb as a general data storage only 
     *  this function is against the philosophy "No delete. Any change by adding New"
     *
     */
    virtual bool RecursiveDeleteVariation(Variation *dir);
    
    //----------------------------------------------------------------------------------------
    //	A S S I G N M E N T S
    //----------------------------------------------------------------------------------------

    /** @brief Get Assignment with data blob only
     *
     * This function is optimized for fast data retrieving and is assumed to be performance critical;
     * This function doesn't return any specified information like variation object or run-range object
     * @see GetAssignmentFull
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] variation - variation name
     * @param [in] loadColumns - optional, do we need to load table columns information (for column names and types) or not
     * @return DAssignment object or NULL if no assignment is found or error
     */
    virtual Assignment* GetAssignmentShort(int run, const string& path, const string& variation="default", bool loadColumns=true)=0;
    
    
    /** @brief Get specified by creation time version of Assignment with data blob only.
     *
     * This function is optimized for fast data retrieving and is assumed to be performance critical;
     * This function doesn't return any specified information like variation object or run-range object
     * The Time is a timestamp, data that is equal or earlier in time than that timestamp is returned
     *
     * @remarks this function is named so
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] time - timestamp, data that is equal or earlier in time than that timestamp is returned
     * @param [in] variation - variation name
     * @param [in] loadColumns - optional, do we need to load table columns information (for column names and types) or not
     * @return DAssignment object or NULL if no assignment is found or error
     */
    virtual Assignment* GetAssignmentShort(int run, const string& path, time_t time, const string& variation="default", bool loadColumns=true)=0;
    
     /** @brief Get specified version of Assignment with data blob only
     *
     * @remarks this function is named so
     * @param [in] run - run number
     * @param [in] path - object path
     * @param [in] version - version is number of constants beginning with first added constant set. So 1 is the first added constants
     * @param [in] variation - variation name
     * @return DAssignment object or NULL if no assignment is found or error
     */
    virtual Assignment* GetAssignmentShortByVersion(int run, const string& path, int version, const string& variation="default")=0;
    

    /** @brief Get last Assignment with all related objects
     *
     * @param     int run
     * @param     path to constant path
     * @return NULL if no assignment is found or error
     */
    virtual Assignment* GetAssignmentFull(int run, const string& path, const string& variation="default")=0;
    
    
    /** @brief  Get specified version of Assignment with all related objects
     *
     * @param     int run
     * @param     const char * path
     * @param     const char * variation
     * @param     int version
     * @return   DAssignment*
     */
    virtual Assignment* GetAssignmentFull(int run, const string& path, int version, const string& variation="default")=0;
        
    /** @brief Creates Assignment using related object
     * 
     * @warning since composing a "NEW" DAssignment is complex operation 
     *          it is suggested to use other reloads of this function for users 
     *          for creating "new" assignments. 
     *          This function is not set as "protected" only to provide easy "copy" operations
     * 
     * @warning this function does not check assignment data consistancy with table nrows and ncolumns
     *
     * This function assumes that all needed data is already contained in assignment
     * object. Including correct type table and data blob. 
     *
     * The function is powerful to copy assignment. For example from one variation to another.
     *
     * The function changes DAssignment object by setting Id and DataVaultId by new values
     *
     * @param    DAssignment * assignment
     * @return   DAssignment* adjusted with Id-s of objects and etc
     */
    virtual bool CreateAssignment(Assignment *assignment) = 0;
    
        /** @brief Creates Assignment using related object
     *
     * Creates Assignment using related object.
     * 
     * Validation:
     * If no such run range found, the new will be created (with no name)
     * No action will be done (and NULL will be returned):
     * -- If no type table with such path exists
     * -- If data is inconsistent with columns number and rows number
     * -- If no variation with such name found
     * 
     * @brief 
     * @param data  		by rows and columns
     * @param runMin		run range minimum
     * @param runMax     run range maximum
     * @param variationName	name of variation
     * @param comments   comments
     * @return NULL if failed, DAssignment reference if success
     */
    virtual Assignment* CreateAssignment(const std::vector<std::vector<std::string> >& data, const std::string& path, int runMin, int runMax, const std::string& variationName, const std::string& comments);

    /** @brief Creates Assignment using related object
     *
     * Creates Assignment using related object.
     * 
     * Validation:
     * If no such run range found, the new will be created (with no name)
     * No action will be done (and false will be returned):
     * -- If no type table with such path exists
     * -- If data is inconsistant with columns number and rows number
     * -- If no variation with such name found
     * 
     * @brief 
     * @param data  		by rows and columns
     * @param runRangeName
     * @param variation	name of vaiation
     * @param comments   comments
     * @return NULL if failed, DAssignment reference if success
     */
    virtual Assignment* CreateAssignment(const vector<vector<string> > &data, const string& path, const string& runRangeName, const string& variationName, const string& comments);
    
    /**
     * @brief Complex and universal function to retrieve assignments
     * 
     * @warning this function is too complex for users. 
     *          Use overloaded GetAssignments instead of it.
     *          And only use this function if others are inapropriate
     * 
     * This function is universal assignments getter. 
     * Provided fields allows to select assignments (arrays and single assignments) 
     * for most cases. Other GetAssignments(...) and GetAssignmentFull(...) 
     * functions relie on this function.  
     *
     * 
     * runMin, runMax:
     *       applied if one !=0. Thus runMin=runMax=0 will select all run ranges
     *       If one needs particular run, runMin=runMax=<NEEDED RUN NUMBER> should be used
     * 
     * runRangeName:
     *       will be ignored if equals to ""
     * 
     * variation:
     *       if "", all variations will be get
     * 
     * date: 
     *       unix timestamp that indicates the latest time to select assignments from
     *       if 0 - time will be ignored
     * sortby:
     *       0 - `assignments`.`created` DESC
     *       1 - `assignments`.`created` ASC
     * 
     * take, startWith
     *       paging parameters
     * 
     * @param [out] assingments result assignment list
     * @param [in] path		  path of type table
     * @param [in] run		  specified range. If not set all ranges
     * @param [in] variation  variation, if not set, all variations
     * @param [in] date		  nearest date
     * @param [in] sortBy	  sortby order
     * @param [in] startWith  record to start with
     * @param [in] take		  records to select
     * @return true if no errors (even if no assignments was selected)
     */
    virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, int runMin, int runMax, const string& runRangeName, const string& variation, time_t beginTime, time_t endTime, int sortBy=0, int take=0, int startWith=0)=0;
    
    
    /**
     * @brief Get assigments for particular run
     * 
     * returns vector of assignments
     * Variation: if variation is not empty string the assignments for specified variation will be returned
     *            otherwise all variations will be accepted
     * 
     * Date: if date is not 0, assignments which are earlier than this date will be returned
     *       otherwise returned assignments will be not filtered by date
     * 
     * Paging: paging could be done with  @see take ans @see startWith 
     *         take=0, startWith=0 means select all records;
     * 
     * 
     * @param [out] assingments
     * @param [in]  path
     * @param [in]  run
     * @param [in]  variation
     * @param [in]  date
     * @param [in]  take
     * @param [in]  startWith
     * @return 
     */
    virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, int run, const string& variation="", time_t date=0, int take=0, int startWith=0)=0;

    /**
     * @brief Get assigments for particular run
     * 
     * returns vector of assignments
     * Variation: if variation is not empty string the assignments for specified variation will be returned
     *            otherwise all variations will be accepted
     * 
     * Date: if date is not 0, assignments which are earlier than this date will be returned
     *       otherwise returned assignments will be not filtered by date
     * 
     * Paging: paging could be done with  @see take ans @see startWith 
     *         take=0, startWith=0 means select all records;
     * 
     *
     * @param [in]  path
     * @param [in]  run
     * @param [in]  variation
     * @param [in]  date
     * @param [in]  take
     * @param [in]  startWith
     * @return assingments
     */
    virtual vector<Assignment *> GetAssignments(const string& path, int run, const string& variation="", time_t date=0, int take=0, int startWith=0)=0;

    /**
     * @brief Get assigments for particular run
     * 
     * returns vector of assignments
     * Variation: if variation is not emty string the assignments for specified variation will be returned
     *            otherwise all variations will be accepted
     * 
     * Date: if date is not 0, assignments wich are earlier than this date will be returned
     *       otherwise returned assignments will be not filtred by date
     * 
     * Paging: paging could be done with  @see take ans @see startWith 
     *         take=0, startWith=0 means select all records;
     * 
     * 
     * @param [out] assingments
     * @param [in]  path
     * @param [in]  run
     * @param [in]  variation
     * @param [in]  date
     * @param [in]  take
     * @param [in]  startWith
     * @return 
     */
    virtual bool GetAssignments(vector<Assignment *> &assingments,const string& path, const string& runName, const string& variation="", time_t date=0, int take=0, int startWith=0)=0;

    /**
     * @brief Get assigments for particular run
     * 
     * returns vector of assignments
     * Variation: if variation is not emty string the assignments for specified variation will be returned
     *            otherwise all variations will be accepted
     * 
     * Date: if date is not 0, assignments wich are earlier than this date will be returned
     *       otherwise returned assignments will be not filtred by date
     * 
     * Paging: paging could be done with  @see take ans @see startWith 
     *         take=0, startWith=0 means select all records;
     * 
     * 
     * @param [out] assingments
     * @param [in]  path
     * @param [in]  run
     * @param [in]  variation
     * @param [in]  date
     * @param [in]  take
     * @param [in]  startWith
     * @return 
     */
    virtual vector<Assignment *> GetAssignments(const string& path, const string& runName, const string& variation="", time_t date=0, int take=0, int startWith=0)=0;
    
    /**
     * @brief the function updates assignment comments
     * @param assignment
     * @return 
     */
    virtual bool UpdateAssignment(Assignment* assignment)=0;
    
    /**
     * @brief Deletes assignment
     * @param assignment object to delete. Must have valid ID 
     * @return true if success
     */
    virtual bool DeleteAssignment(Assignment* assignment)=0;
    
    /**
     * @brief Fill assignment with data if it has proper ID
     * 
     * The function actually gets assignment by ID. 
     * 
     * A problem: is that for DB providers the id is assignment->GetId().
     *            For file provider the id is probably a type table full path
     * A solution: 
     *    is to have this function that accepts DAssignment* assignment.
     *    DAssignment* assignment incapsulates the id (one way or another)
     *    And each provider could check if this DAssignment have valid Id,
     *    fill assignment with data and return true. 
     *    Or just return "false" if something goes wrong;
     * 
     * @param [in, out] assignment to fill
     * @return true if success and assignment was filled with data
     */
    virtual bool FillAssignment(Assignment* assignment)=0;
    
    //----------------------------------------------------------------------------------------
    //	E R R O R   H A N D L I N G 
    //----------------------------------------------------------------------------------------
    /**
     * @brief Get number of errors 
     * @return 
     */
    virtual int GetNErrors();
    
    /**
     * @brief Get vector of last errors 
     */
    virtual const std::vector<int>& GetErrorCodes();
    
    /** @brief return vector of errors.
     * @warning the error objects are deleted on next function that clears errors. 
     * (!) Copy this error objects before next provider function call. 
     * @return   std::vector<DCcdbError *>
     */
    virtual std::vector<CCDBError *> GetErrors();
    
    /**
     * @brief Gets last of the last error
     * @return error code
     */
    virtual int GetLastError();
    
    /** @brief Logs error 
    *
    * @param errorCode Error codes see DCCDBGlobals.h
    * @param module	Caller should specify method name here
    * @param message	Message of the error
    * @return   void
    */
    virtual void Error(int errorCode, const std::string& module, const std::string& message);

    /** @brief Logs error 
    *
    * @param errorCode Error codes see DCCDBGlobals.h
    * @param module	Caller should specify method name here
    * @param message	Message of the error
    * @return   void
    */
    virtual void Warning(int errorCode, const std::string& module, const std::string& message);

    /** @brief Clears Errors
     * function is called on start of each function that produce errors
     * @return   void
     */
    virtual void ClearErrors();
    
    //----------------------------------------------------------------------------------------
    //	O T H E R   F U N C T I O N S
    //----------------------------------------------------------------------------------------
    
    /** @brief Validates name for constant type table or directory or column
     *
     * @param     string name
     * @return   bool
     */
    bool ValidateName(const string& name);



    //----------------------------------------------------------------------------------------
    //	L O G G I N G
    //----------------------------------------------------------------------------------------
    std::string GetLogUserName() const { return mLogUserName; }		 ///User name for logging
    void SetLogUserName(std::string val) { mLogUserName = val; } ///User name for logging

    
    
protected:
    /** @brief Sets IsLoaded() and  resets IsChanged()Yt
     *
     * @param     obj
     * @return   void
     */
    void SetObjectLoaded(StoredObject* obj);
    
    
    /** @brief Clear error state on start of each function that emits error
     */
    virtual void ClearErrorsOnFunctionStart();
    
    vector<int> mErrorCodes;		///vector of last errors
    vector<CCDBError *> mErrors;	///errors 
    
    int mLastError;		///last error
    
    const int mMaximumErrorsToHold; ///=100 Maximum errors to hold in @see mLastErrors
    
    std::string mLogUserName; ///User name

    std::string mConnectionString; ///Connection string that was used on last successfull connect.
};
}
#endif // _DDataProvider_


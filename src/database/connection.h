/*
* This file defined connection of database which is based on sqlite.
*/

#ifndef DATABASE_CONNECTION_H__
#define DATABASE_CONNECTION_H__

#include "base/base_types.h"
#include "sqlite3/sqlite3.h"
#include "base/time/time.h"

namespace database {
	class Connection : public base::noncopyable {
	public:
		// The OpenFlags can build up a option to  indicate in which hopes you want to
		// open the database.
		enum OpenFlags {
			kReadOnly = SQLITE_OPEN_READONLY,       // Read only
			kReadWrite = SQLITE_OPEN_READWRITE,      // Read and write mode, it will failed when the file is not exist 
			kCreate = SQLITE_OPEN_CREATE,         // If the file not exist then create it. you can use it only with modeReadWrite
			kMultiThread   = SQLITE_OPEN_NOMUTEX,        // No mutex mode, different thread can operate different db connection
			kSerialized    = SQLITE_OPEN_FULLMUTEX,      // Mutex mode, different thread can operate one db connection
		};

		Connection();
		~Connection();
		// Open database with special flags.
		bool Open(int open_flags = kReadWrite | kCreate | kSerialized);
		bool Open(const char *filename, int open_flags = kReadWrite | kCreate | kSerialized);
		bool WasOpened() {
			return !!db_;
		}

		void Close();
		void Compact();
		bool Execute(const char *sql_text, base::TimeSpan time_out = TimeSpan());
		bool IsValid();
		// Get last error code associated with the last sqlite operation..
		int GetErrorCode() const;
		// Get the errno associated with the last last error code.
		int GetLastErrno() const;
		// Get the description about the last error.
		const char* GetErrorMessage() const;
		int64_t GetLastInsertRowId() const;
		int GetLastChangeCount() const;
	private:
		bool OpenInternal(const char *filename);
		sqlite3* db_;
	};
}

#endif
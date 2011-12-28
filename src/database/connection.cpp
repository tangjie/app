#include "database/connection.h"

namespace {
	class AutoConfigTimeout {
	public:
		AutoConfigTimeout(sqlite3 *db) : db_(db) {
		}

		~AutoConfigTimeout() {
			sqlite3_busy_timeout(db_, 0);
		}

		int SetTimeout(base::TimeSpan time_out) {
			assert(time_out.ToMilliseconds() < INT_MAX);
			return sqlite3_busy_timeout(db_, static_cast<int>(time_out.ToMilliseconds()));
		}

	private:
		sqlite3 *db_;
	};
}

namespace database {
	Connection::Connection() : db_(nullptr) {

	}

	Connection::~Connection() {
		Interrupt();
		Close();
	}

	bool Connection::Open(const char *filename, int open_flags) {
		Interrupt();
		Close();
		if (nullptr == filename) {
			return false;
		}

		int rc = sqlite3_open_v2(filename, &db_, open_flags, nullptr);
		if (SQLITE_OK != rc) {
			Close();
		}
		return SQLITE_OK == rc;
	}

	bool Connection::Open(int open_flags) {
		// use mem_db to open database in memory.
		const char *mem_db = ":memory:";
		return Open(mem_db, open_flags);
	}

	bool Connection::Close() {
		if (nullptr != db_) {
			int rc = sqlite3_close(db_);
			if (SQLITE_OK == rc) {
				db_ = nullptr;
			}
			return SQLITE_OK == rc;
		}
		return false;
	}

	bool Connection::Compact() {
		const char *sql_text = "VACUUM";
		return SQLITE_OK == Execute(sql_text);
	}

	bool Connection::Execute(const char *sql_text) {
		if (nullptr == db_) {
			return false;
		}
		return SQLITE_OK == sqlite3_exec(db_, sql_text, nullptr, nullptr, nullptr);
	}

	bool Connection::ExecuteWithTimeout(const char *sql_text, base::TimeSpan time_out) {
		if (nullptr == db_) {
			return false;
		}
		AutoConfigTimeout auto_time_out(db_);
		auto_time_out.SetTimeout(time_out);
		return SQLITE_OK == sqlite3_exec(db_, sql_text, nullptr, nullptr, nullptr);
	}

	void Connection::Interrupt() {
		if (nullptr != db_) {
			sqlite3_interrupt(db_);
		}
	}

	int Connection::GetErrorCode() const {
		if (nullptr ==db_) {
			return SQLITE_ERROR;
		}
		return sqlite3_errcode(db_);
	}

	const char* Connection::GetErrorMessage() const {
		const char *default_error_message = "connection is invalid";
		if (nullptr == db_) {
			return default_error_message;
		}
		return sqlite3_errmsg(db_);
	}

	int Connection::GetLastChangeCount() const {
		if (nullptr == db_) {
			return 0;
		}
		return sqlite3_changes(db_);
	}

	int Connection::GetLastErrno() const {
		if (nullptr == db_) {
			return -1;
		}
		int err = 0;
		if (SQLITE_OK != sqlite3_file_control(db_, nullptr, SQLITE_LAST_ERRNO, &err)) {
			return -2;
		}
		return err;
	}

	int64_t Connection::GetLastInsertRowId() const {
		if (nullptr == db_) {
			return 0;
		}
		return sqlite3_last_insert_rowid(db_);
	}
}
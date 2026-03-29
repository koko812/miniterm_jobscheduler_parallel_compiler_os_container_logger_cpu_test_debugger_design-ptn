ts=$(date +%Y%m%d_%H%M%S)
cp sns.db "db_backups/sns_${ts}.db"

sqlite3 "db_backups/sns_${ts}.db" <<'SQL' > "db_backups/sns_${ts}.status.txt"
.print 'tables'
.tables

.print 'schema'
.schema

.print 'integrity_check'
PRAGMA integrity_check;

.print 'users'
SELECT COUNT(*) FROM users;

.print 'posts'
SELECT COUNT(*) FROM posts;

.print 'follows'
SELECT COUNT(*) FROM follows;

.print 'page_count'
PRAGMA page_count;

.print 'page_size'
PRAGMA page_size;

.print 'journal_mode'
PRAGMA journal_mode;
SQL

using Dapper;
using ExamsAPI.Models;
using System.Data;
using System.Data.SqlClient;

namespace ExamsAPI.Services
{
    public class ExamService : IExamService
    {
        private readonly string _connectionString;

        public ExamService(ExamServiceOptions options)
        {
            _connectionString = options.ConnectionString;
        }

        public async Task<List<Exam>> GetAsync()
        {
            using (IDbConnection db = new SqlConnection(_connectionString))
            {
                return (await db.QueryAsync<Exam>("SELECT * FROM Exams")).ToList();
            }
        }

        public async Task<Exam?> GetAsync(int id)
        {
            using (IDbConnection db = new SqlConnection(_connectionString))
            {
                return (await db.QueryAsync<Exam>("SELECT * FROM Exams WHERE ExamId = @id", new { id })).FirstOrDefault();
            }
        }

        public async Task<Exam> CreateAsync(Exam exam)
        {
            using (IDbConnection db = new SqlConnection(_connectionString))
            {
                var sqlQuery = """INSERT INTO Exams (ExamName, StudentName, Date, Mark, "Group") VALUES(@ExamName, @StudentName, @Date, @Mark, @Group); SELECT CAST(SCOPE_IDENTITY() as int)""";
                int? examId = (await db.QueryAsync<int>(sqlQuery, exam)).FirstOrDefault();
                exam.ExamId = examId.Value;
            }

            return exam;
        }

        public async Task UpdateAsync(Exam exam)
        {
            using (IDbConnection db = new SqlConnection(_connectionString))
            {
                var sqlQuery = """UPDATE Exams SET ExamName = @ExamName, StudentName = @StudentName, Date = @Date, Mark = @Mark, "Group" = @Group WHERE ExamId = @ExamId""";
                await db.ExecuteAsync(sqlQuery, exam);
            }
        }

        public async Task DeleteAsync(int id)
        {
            using (IDbConnection db = new SqlConnection(_connectionString))
            {
                var sqlQuery = "DELETE FROM Exams WHERE ExamId = @id";
                await db.ExecuteAsync(sqlQuery, new { id });
            }
        }
    }
}

using ExamsAPI.Models;

namespace ExamsAPI.Services
{
    public interface IExamService
    {
        Task<Exam> CreateAsync(Exam exam);
        Task DeleteAsync(int id);
        Task<List<Exam>> GetAsync();
        Task<Exam?> GetAsync(int id);
        Task UpdateAsync(Exam exam);
    }
}
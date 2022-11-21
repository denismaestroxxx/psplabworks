namespace ExamsAPI.Models
{
    public class Exam
    {
        public int ExamId { get; set; }

        public required string ExamName { get; set; }

        public required string StudentName { get; set; }

        public DateTime Date { get; set; }

        public int Mark { get; set; }

        public required string Group { get; set; }
    }
}

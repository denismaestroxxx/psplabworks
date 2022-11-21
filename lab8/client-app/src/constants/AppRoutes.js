export const AppRoutes = {
    home: '/',
    manageExams: '/exams/manageExams',
    createExam: "/exams/createExam",
    updateExam: "/exams/updateExam/:examId",
    updateExamFunc: (examId) => {return `/exams/updateExam/${examId}`}
}
export class ExamService {
    examsApiHost = "http://localhost:5203/";

    async getExams() {
            const resp = await fetch(this.examsApiHost +  "exams", {
                method: "GET",
            });
            const data = await resp.json();
            if (resp.ok && data) {
                return data;
            }
        return [];
    }
    async getExamById(id) {
            const resp = await fetch(this.examsApiHost + "exams/" + id, {
                method: "GET"
            });
            const data = await resp.json();
            if (resp.ok && data) {
                return data;
            }
        return null;

    }
    async createExam(exam){
            const resp = await fetch(this.examsApiHost + "exams/", {
                method: "POST",
                headers: {
                    'Content-Type': 'application/json;charset=utf-8'
                  },
                body: JSON.stringify(exam)
            });
            if (resp.ok) {
                return true;
            }
        return false;
    }
    async updateExam(exam){
        const resp = await fetch(this.examsApiHost + "exams/", {
            method: "PUT",
            headers: {
                'Content-Type': 'application/json;charset=utf-8'
              },
            body: JSON.stringify(exam)
        });
        if (resp.ok) {
            return true;
        }
    return false;
   }
   
    async deleteExam(id) {
            const resp = await fetch(this.examsApiHost + "exams/" + id, {
                method: "DELETE"
            });
            if (resp.ok) {
                return true;
            }
        return false;
    }
}
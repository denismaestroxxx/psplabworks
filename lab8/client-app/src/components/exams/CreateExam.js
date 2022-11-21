import React, { useState } from 'react';
import { ExamService } from '../../services/ExamService';
import { AppRoutes } from '../../constants/AppRoutes';

export function CreateExam() {
    const [examName, setExamName] = useState(null);
    function handleExamNameChange(e){
        setExamName(e.target.value);
    }
    const [studentName, setStudentName] = useState(null);
    function handleStudentNameChange(e){
        setStudentName(e.target.value);
    }
    const [date, setDate] = useState(null);
    function handleDateChange(e){
        setDate(e.target.value);
    }
    const [mark, setMark] = useState(null);
    function handleMarkChange(e){
        setMark(e.target.value);
    }
    const [group, setGroup] = useState(null);
    function handleGroupChange(e){
        setGroup(e.target.value);
    }

    function handleSubmit(){
       let examToCreate = { examName: examName, studentName: studentName, date: date, mark: mark, group: group };
       let service = new ExamService();
       service.createExam(examToCreate).then(() => window.location.replace(AppRoutes.manageExams));
    }

    return (
            <div className="all-container">
            <div className="form">
                <label >Exam Name</label>
                <input onChange={handleExamNameChange} value={examName} type="text" className="form-input" placeholder="Exam name.." />

                <label>Student full Name</label>
                <input onChange={handleStudentNameChange} value={studentName} type="text" className="form-input" placeholder="Student full name.." />

                <label>Date</label>
                <input onChange={handleDateChange} value={date} type="datetime-local" className="form-input" />

                <label>Mark</label>
                <input onChange={handleMarkChange} value={mark} type="number" min="1" max="10" className="form-input" placeholder="Mark.." />

                <label>Group</label>
                <input onChange={handleGroupChange} value={group} type="text" className="form-input" placeholder="Group.." />
                <button className="form-submit" onClick={handleSubmit}>Create</button>
            </div>
            </div>
    );
}
import React, { useState, useEffect } from 'react';
import { ExamService } from '../../services/ExamService';
import { useParams } from 'react-router-dom';
import { AppRoutes } from '../../constants/AppRoutes';

export function UpdateExam() {
    const { examId } = useParams();
    const [exam, setExam] = useState({});
    const [loading, setLoading] = useState(true);
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
    useEffect(() => {
        async function getExam() {
            let service = new ExamService();
            let foundExam = await service.getExamById(examId);
            setLoading(false);
            setExam(foundExam);
            setExamName(foundExam.examName);
            setStudentName(foundExam.studentName);
            setDate(foundExam.date);
            setMark(foundExam.mark);
            setGroup(foundExam.group);
        };

        if (loading === true) {
            getExam();
        }
    });

    function handleSubmit(){
       let examToUpdate = exam;
       examToUpdate.examName = examName;
       examToUpdate.studentName = studentName;
       examToUpdate.date = date;
       examToUpdate.mark = mark;
       examToUpdate.group = group;
       let service = new ExamService();
       service.updateExam(examToUpdate).then(() => window.location.replace(AppRoutes.manageExams));
    }

    return (
        loading
            ? <p><em>Loading...</em></p>
            :
            <div className="all-container">
            <div className="form">
                <label >Exam Name</label>
                <input onChange={handleExamNameChange} value={examName} type="text" className="form-input" placeholder="Exam name.." />

                <label>Student full name</label>
                <input onChange={handleStudentNameChange} value={studentName} type="text" className="form-input" placeholder="Student full name.." />

                <label>Exam date</label>
                <input onChange={handleDateChange} value={date} type="datetime-local" className="form-input" />

                <label>Mark</label>
                <input onChange={handleMarkChange} value={mark} type="number"  min="1" max="10"  className="form-input" placeholder="Mark.." />

                <label>Group</label>
                <input onChange={handleGroupChange} value={group} type="text" className="form-input" placeholder="Group.." />
                <button className="form-submit" onClick={handleSubmit}>Update</button>
            </div>
            </div>
    );
}
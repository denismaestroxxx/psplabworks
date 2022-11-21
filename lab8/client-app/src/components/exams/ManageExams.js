import React, { Component } from 'react';
import { ExamService } from '../../services/ExamService';
import { NavLink} from "react-router-dom";
import {AppRoutes} from '../../constants/AppRoutes';

export class ManageExams extends Component {
    static displayName = ManageExams.name;

    constructor(props) {
        super(props);
        this.state = { exams: [], loading: true };
    }

    async componentDidMount() {
        let service = new ExamService();
        const data = await service.getExams();
        this.setState({ exams: data, loading: false });
    }

    handleDeleteClick = async (e, id) =>{
        let service = new ExamService();
        await service.deleteExam(id);
        const data = await service.getExams();
        this.setState({ exams: data, loading: false });
    }

    renderExams(exams) {
        return (
                <div className="all-container">
                    <div className="container">
                        <div className="container-item">
                        <NavLink className="details-button" to={AppRoutes.createExam} state={ {back: AppRoutes.manageExams }}>Create</NavLink>
                        </div>
                        <div className="container-item">
                            {exams.length > 0 ?
                                <table className="table">
                                    <thead>
                                        <tr>
                                            <th>Exam name</th>
                                            <th>Student full name</th>
                                            <th>Date</th>
                                            <th>Mark</th>
                                            <th>Group</th>
                                            <th>Options</th>
                                        </tr>
                                    </thead>
                                    <tbody>

                                        {exams.map(item =>
                                        
                                            <tr key={item.examId}>
                                                <td>{item.examName}</td>
                                                <td>{item.studentName}</td>
                                                <td>{item.date}</td>
                                                <td>{item.mark}</td>
                                                <td>{item.group}</td>
                                                <td>
                                                    <div className="options-container">
                                                    <div className="option">
                                                    <NavLink className="details-button" to={AppRoutes.updateExamFunc(item.examId)} state={ {back: AppRoutes.manageExams }}>Update</NavLink>
                                                    </div>
                                                    <div className="option">
                                                    <button className="details-button delete-button" onClick={(e) => this.handleDeleteClick(e, item.examId)}>Delete</button>
                                                    </div>
                                                    </div>
                                                </td>
                                            </tr>
                                        )}
                                    </tbody>
                                </table>
                                :
                                <h3>There are no exams</h3>}
                        </div>
                    </div>
                </div>
        );
    }

    render() {
        let contents = this.state.loading
            ? <p><em>Loading...</em></p>
            : this.renderExams(this.state.exams);

        return (
                <>
                {contents}
                </>
        );
    }
}
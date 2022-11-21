import './App.css';
import React from 'react';
import {
  BrowserRouter,
  Routes,
  Route,
} from "react-router-dom";
import {Home} from './components/home/Home';
import {ManageExams} from './components/exams/ManageExams';
import {CreateExam} from './components/exams/CreateExam';
import {UpdateExam} from './components/exams/UpdateExam';
import {AppRoutes} from './constants/AppRoutes';

export function App() {

  return (
    <BrowserRouter>
      <Routes>
          <Route path={AppRoutes.home} element={<Home />} />
          <Route path={AppRoutes.manageExams} element={<ManageExams />} />
          <Route path={AppRoutes.createExam} element={<CreateExam />} />
          <Route path={AppRoutes.updateExam} element={<UpdateExam />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
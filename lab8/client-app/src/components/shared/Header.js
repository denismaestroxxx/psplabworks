import React, { Component } from 'react';
import {AppRoutes} from '../../constants/AppRoutes';
import mainLogo from '../../content/main-logo.png';

export class Header extends Component {
    static displayName = Header.name;

    render () {
    return (
    <header className="header">
      <div className="header-item">
        <div className="logo">
        <a href={AppRoutes.home}><img className="logo-img" src={mainLogo} alt="" /></a>
        </div>
        <div className="header-title">
          <h1>Administration tool</h1>
        </div>
      </div>
      <nav className="navbar">
        <a href={AppRoutes.manageExams}>Exams </a>
      </nav>
    </header>
    );
  }
}
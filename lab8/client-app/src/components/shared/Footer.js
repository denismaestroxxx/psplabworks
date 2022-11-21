import React, { Component } from 'react';

export class Footer extends Component {
    static displayName = Footer.name;

    render () {
    return (
        <footer className="footer">
        <div className="footer-item">
          <h3>© Doroshko Denis 2022.</h3>
        </div>
      </footer>
    );
  }
}
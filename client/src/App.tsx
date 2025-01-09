import "./App.css";
import Layout from "./app/Layout";
import {
  Route,
  BrowserRouter as Router,
  Routes,
  Navigate,
} from "react-router-dom";
import Login from "./page/auth/Login";

function App() {
  return (
    <Router>
      <Routes>
        <Route path="*" element={<Navigate to="/app" />} />
        <Route path="/auth/login" element={<Login />} />
      </Routes>
    </Router>
  );
}

export default App;

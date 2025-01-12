import "./App.css";
import {
  Route,
  BrowserRouter as Router,
  Routes,
  Navigate,
} from "react-router-dom";
import Login from "@/page/auth/Login";
import Layout from "./app/Layout";

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Layout />} />
        <Route path="/auth/login" element={<Login />} />
        <Route path="*" element={<Navigate to="/" />} />
      </Routes>
    </Router>
  );
}

export default App;

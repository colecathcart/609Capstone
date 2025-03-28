import { useState } from "react";
import { motion } from "framer-motion";
import { useNavigate } from 'react-router-dom';

const LoginPage = ({ onLogin }) => {
    const navigate = useNavigate();
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [error, setError] = useState("");

    const handleSubmit = (e) => {
    e.preventDefault();

    if (username === "admin" && password === "admin") {
        navigate("/overview");
    } else {
        setError("Invalid credentials. Try again.");
    }
    };

    return (
    <div className="fixed inset-0 flex items-start justify-center pt-64 bg-gray-900 text-white">
        <motion.div
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        transition={{ duration: 0.5 }}
        className="bg-gray-800 bg-opacity-70 p-8 rounded-xl shadow-xl border border-gray-700 w-full max-w-md"
        >
        <h2 className="text-2xl font-semibold mb-6 text-center">Login</h2>
        {error && (
            <div className="text-red-400 text-center mb-4">{error}</div>
        )}
        <form onSubmit={handleSubmit} className="space-y-4">
            <input
            className="w-full p-3 rounded-lg bg-gray-700 text-white placeholder-gray-400 focus:outline-none focus:ring-2 focus:ring-blue-500"
            placeholder="Username"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            />

            <input
            type="password"
            className="w-full p-3 rounded-lg bg-gray-700 text-white placeholder-gray-400 focus:outline-none focus:ring-2 focus:ring-blue-500"
            placeholder="Password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            />

            <button
            type="submit"
            className="w-full p-3 bg-blue-600 hover:bg-blue-500 rounded-lg font-semibold transition"
            >
            Log In
            </button>
        </form>
        </motion.div>
    </div>
    );
};

export default LoginPage;

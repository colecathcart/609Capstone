import { useState } from "react";
import { Monitor, ShieldCheck, ShieldAlert, ShieldX, Settings } from "lucide-react";
import { motion } from "framer-motion";
import { useNavigate } from "react-router-dom";

import Header from "../components/common/Header";
import StatCard from "../components/common/StatCard";
import ClientGrid from "../components/overview/ClientGrid";
import DetectedRansomwareTable from "../components/overview/DetectedRansomwareTable";

const OverviewPage = () => {
	const navigate = useNavigate();
	const [dropdownOpen, setDropdownOpen] = useState(false);

	const handleLogout = () => {
		navigate("/");
	};

	return (
		<div className="flex-1 overflow-auto relative z-10">
			<Header title="System Overview" />

			{/* Settings Gear */}
			<div className="absolute top-4 right-8">
				<div className="relative">
					<button
						className="p-2 rounded-full bg-gray-700 hover:bg-gray-600 transition"
						onClick={() => setDropdownOpen(!dropdownOpen)}
					>
						<Settings className="text-white" size={20} />
					</button>

					{dropdownOpen && (
						<div className="absolute right-0 mt-2 w-32 bg-gray-800 shadow-lg rounded-lg border border-gray-700">
							<button
								className="w-full px-4 py-2 text-left text-white hover:bg-gray-600 transition"
								onClick={handleLogout}
							>
								Logout
							</button>
						</div>
					)}
				</div>
			</div>

			<main className="max-w-7xl mx-auto py-6 px-4 lg:px-8">
				{/* STATS */}
				<motion.div
					className="grid grid-cols-1 gap-5 sm:grid-cols-2 lg:grid-cols-4 mb-8"
					initial={{ opacity: 0, y: 20 }}
					animate={{ opacity: 1, y: 0 }}
					transition={{ duration: 1 }}
				>
					<StatCard name="Threats Detected" icon={ShieldAlert} value="450" color="rgb(73, 130, 216)" />
					<StatCard name="Processes Killed" icon={ShieldCheck} value="400" color="rgb(73, 130, 216)" />
					<StatCard name="Devices" icon={Monitor} value="6" color="rgb(73, 130, 216)" />
					<StatCard name="Compromises" icon={ShieldX} value="50" color="rgb(73, 130, 216)" />
				</motion.div>

				{/* CHARTS */}
				<div className="grid grid-cols-1 lg:grid-cols-2 gap-8 mb-8">
					<ClientGrid />
					<DetectedRansomwareTable />
				</div>
			</main>
		</div>
	);
};

export default OverviewPage;

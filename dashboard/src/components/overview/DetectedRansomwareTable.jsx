import { motion } from "framer-motion";
import { Search } from "lucide-react";
import { useState } from "react";

const RANSOMWARE_DATA = [
	{ id: 1, name: "GonnaCry", signature: "SHA256: b4f6...7d1a", lastSeen: "2025-03-20" },,
	{ id: 5, name: "Lockbit", signature: "SHA256: a1b2...c3d4", lastSeen: "2025-03-10" },
];

const DetectedRansomwareTable = () => {
	const [searchTerm, setSearchTerm] = useState("");
	const [filteredData, setFilteredData] = useState(RANSOMWARE_DATA);

	const handleSearch = (e) => {
		const term = e.target.value.toLowerCase();
		setSearchTerm(term);
		const filtered = RANSOMWARE_DATA.filter(
			(item) => item.name.toLowerCase().includes(term) || item.signature.toLowerCase().includes(term)
		);
		setFilteredData(filtered);
	};

	return (
		<motion.div
			className='bg-gray-800 bg-opacity-50 backdrop-blur-md shadow-lg rounded-xl p-6 border border-gray-700 mb-8 h-full flex flex-col'
			initial={{ opacity: 0, y: 20 }}
			animate={{ opacity: 1, y: 0 }}
			transition={{ delay: 0.2 }}
		>
			<div className='flex justify-between items-center mb-6'>
				<h2 className='text-xl font-semibold text-gray-100'>Detected Ransomware</h2>
				<div className='relative'>
					<input
						type='text'
						placeholder='Search ransomware...'
						className='bg-gray-700 text-white placeholder-gray-400 rounded-lg pl-10 pr-4 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500'
						onChange={handleSearch}
						value={searchTerm}
					/>
					<Search className='absolute left-3 top-2.5 text-gray-400' size={18} />
				</div>
			</div>

			<div className='overflow-x-auto'>
				<table className='min-w-full divide-y divide-gray-700'>
					<thead>
						<tr>
							<th className='px-6 py-3 text-left text-xs font-medium text-gray-400 uppercase tracking-wider'>
								Name
							</th>
							<th className='px-6 py-3 text-left text-xs font-medium text-gray-400 uppercase tracking-wider'>
								Signature
							</th>
							<th className='px-6 py-3 text-left text-xs font-medium text-gray-400 uppercase tracking-wider'>
								Last Seen
							</th>
						</tr>
					</thead>

					<tbody className='divide-y divide-gray-700'>
						{filteredData.map((item) => (
							<motion.tr
								key={item.id}
								initial={{ opacity: 0 }}
								animate={{ opacity: 1 }}
								transition={{ duration: 0.3 }}
							>
								<td className='px-6 py-4 whitespace-nowrap text-sm font-medium text-gray-100'>
									{item.name}
								</td>
								<td className='px-6 py-4 whitespace-nowrap text-sm text-gray-300'>{item.signature}</td>
								<td className='px-6 py-4 whitespace-nowrap text-sm text-gray-300'>{item.lastSeen}</td>
							</motion.tr>
						))}
					</tbody>
				</table>
			</div>
		</motion.div>
	);
};
export default DetectedRansomwareTable;

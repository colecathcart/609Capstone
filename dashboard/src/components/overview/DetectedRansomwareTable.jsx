// DetectedRansomwareTable.jsx
import { motion } from "framer-motion";
import { Search } from "lucide-react";
import { useState } from "react";

const DetectedRansomwareTable = ({ blacklisted }) => {
  const [searchTerm, setSearchTerm] = useState("");

  // Helper function to shorten long hashes
  const shortenHash = (fullHash) => {
    if (!fullHash || fullHash.length <= 25) {
      return fullHash; // No need to shorten
    }
    const start = fullHash.slice(0, 10);
    const end = fullHash.slice(-10);
    return `${start}...${end}`;
  };

  // Filter the data by hash text only
  const filteredData = blacklisted.filter((item) =>
    item.hash.toLowerCase().includes(searchTerm.toLowerCase())
  );

  return (
    <motion.div
      className="bg-gray-800 bg-opacity-50 backdrop-blur-md shadow-lg rounded-xl p-6 border border-gray-700 mb-8 h-full flex flex-col"
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ delay: 0.2 }}
    >
      <div className="flex justify-between items-center mb-6">
        <h2 className="text-xl font-semibold text-gray-100">
          Blacklisted Hashes
        </h2>
        <div className="relative">
          <input
            type="text"
            placeholder="Search hashes..."
            className="bg-gray-700 text-white placeholder-gray-400 rounded-lg pl-10 pr-4 py-2 focus:outline-none focus:ring-2 focus:ring-blue-500"
            onChange={(e) => setSearchTerm(e.target.value)}
            value={searchTerm}
          />
          <Search className="absolute left-3 top-2.5 text-gray-400" size={18} />
        </div>
      </div>

      <div className="overflow-x-auto">
        <table className="min-w-full divide-y divide-gray-700">
          <thead>
            <tr>
              <th className="px-6 py-3 text-left text-xs font-medium text-gray-400 uppercase tracking-wider">
                Hash
              </th>
              <th className="px-6 py-3 text-left text-xs font-medium text-gray-400 uppercase tracking-wider">
                Last Seen
              </th>
            </tr>
          </thead>
          <tbody className="divide-y divide-gray-700">
            {filteredData.map((item, index) => (
              <motion.tr
                key={index}
                initial={{ opacity: 0 }}
                animate={{ opacity: 1 }}
                transition={{ duration: 0.3 }}
              >
                <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-100">
                  {shortenHash(item.hash)}
                </td>
                <td className="px-6 py-4 whitespace-nowrap text-sm text-gray-100">
                  {item.lastSeen}
                </td>
              </motion.tr>
            ))}
          </tbody>
        </table>
      </div>
    </motion.div>
  );
};

export default DetectedRansomwareTable;

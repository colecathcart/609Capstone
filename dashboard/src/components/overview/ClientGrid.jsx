import { useState } from "react";
import { motion } from "framer-motion";
import QuarantineModal from "./QuarantineModal";

const statusColors = {
  "Threat Detected": "bg-red-800",
  Online: "bg-yellow-700",
  Offline: "bg-gray-700",
  Disconnected: "bg-gray-800",
};

const ClientGrid = ({ clients }) => {
  const [selectedDevice, setSelectedDevice] = useState(null);

  // Hardcoded disconnected clients
  const hardcodedClients = [
    { name: "Louis-PC", status: "Disconnected" },
    { name: "Yves-Laptop", status: "Disconnected" },
  ];

  // Combine hardcoded clients with live ones
  const displayClients = [...clients, ...hardcodedClients];

  return (
    <motion.div
      className="bg-gray-800 bg-opacity-70 backdrop-blur-md shadow-lg rounded-xl p-6 border border-gray-700 h-full flex flex-col relative"
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ delay: 0.4 }}
    >
      <h2 className="text-xl font-semibold mb-4 text-gray-100">Device Status</h2>

      <div className="grid grid-cols-2 md:grid-cols-3 gap-4 overflow-y-auto max-h-72">
        {displayClients.map((client, index) => (
          <div
            key={index}
            className={`rounded-xl p-4 flex flex-col justify-center shadow-md ${
              statusColors[client.status] || "bg-gray-700"
            } bg-gradient-to-br from-white/5 to-transparent ${
              client.status === "Threat Detected"
                ? "cursor-pointer hover:ring-2 hover:ring-white transition"
                : ""
            }`}
            onClick={() =>
              client.status === "Threat Detected" && setSelectedDevice(client)
            }
          >
            <span className="font-semibold text-lg">{client.name}</span>
            <span className="text-sm font-medium">{client.status}</span>
          </div>
        ))}
      </div>

      {selectedDevice && (
        <QuarantineModal
          deviceName={selectedDevice.name}
          onConfirm={() => setSelectedDevice(null)}
          onCancel={() => setSelectedDevice(null)}
        />
      )}
    </motion.div>
  );
};

export default ClientGrid;

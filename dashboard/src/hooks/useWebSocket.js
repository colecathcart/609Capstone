import { useEffect, useState } from "react";

const useWebSocket = () => {
  const [data, setData] = useState(null);

  useEffect(() => {

    const socket = new WebSocket(import.meta.env.VITE_WEBSOCKET_URI);

    socket.onmessage = (event) => {
      try {
        const parsed = JSON.parse(event.data);
        setData(parsed);
      } catch (err) {
        console.error("WebSocket parse error:", err);
      }
    };

    socket.onerror = (err) => console.error("WebSocket error:", err);
    return () => socket.close();
  }, []);

  return data;
};

export default useWebSocket;

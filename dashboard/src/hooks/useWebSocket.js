import { useEffect, useState } from "react";

const useWebSocket = (url) => {
  const [data, setData] = useState(null);

  useEffect(() => {
    const socket = new WebSocket(url);

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
  }, [url]);

  return data;
};

export default useWebSocket;

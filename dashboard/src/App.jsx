import { Route, Routes } from "react-router-dom";
import LoginPage from "./pages/LoginPage";
import OverviewPage from "./pages/OverviewPage";

function App() {
  return (
    <Routes>
      <Route path='/' element={<LoginPage />} />
      <Route
        path='/overview'
        element={
          <div className='flex h-screen bg-gray-900 text-gray-100 overflow-hidden'>
            <div className='flex-1 overflow-auto relative z-10'>
              <OverviewPage />
            </div>
          </div>
        }
      />
    </Routes>
  );
}

export default App;

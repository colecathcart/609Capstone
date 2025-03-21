const QuarantineModal = ({ deviceName, onConfirm, onCancel }) => {
    return (
      <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50">
        <div className="bg-gray-800 rounded-lg p-6 shadow-lg border border-gray-700 text-center">
          <h3 className="text-lg font-semibold text-gray-100 mb-4">
            Quarantine device "{deviceName}"?
          </h3>
          <div className="flex justify-center gap-4">
            <button
              className="bg-red-600 hover:bg-red-500 text-white px-4 py-2 rounded"
              onClick={onConfirm}
            >
              Quarantine
            </button>
            <button
              className="bg-gray-600 hover:bg-gray-500 text-white px-4 py-2 rounded"
              onClick={onCancel}
            >
              Cancel
            </button>
          </div>
        </div>
      </div>
    );
  };
  
  export default QuarantineModal;
  
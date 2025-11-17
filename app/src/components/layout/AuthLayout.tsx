import { Outlet } from 'react-router-dom';
import { ChefHat } from 'lucide-react';

export default function AuthLayout() {
  return (
    <div className="min-h-screen bg-gradient-to-br from-primary-50 to-white flex items-center justify-center p-4">
      <div className="w-full max-w-md">
        <div className="text-center mb-8">
          <div className="flex justify-center mb-4">
            <div className="h-16 w-16 bg-primary-600 rounded-2xl flex items-center justify-center">
              <ChefHat className="h-10 w-10 text-white" />
            </div>
          </div>
          <h1 className="text-3xl font-bold text-gray-900">Y Qué Comemos</h1>
          <p className="text-gray-600 mt-2">Planifica tus comidas de forma inteligente</p>
        </div>
        <Outlet />
      </div>
    </div>
  );
}

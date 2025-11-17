import { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import {
  Package,
  ChefHat,
  Calendar,
  TrendingUp,
  AlertCircle,
  Clock
} from 'lucide-react';
import { useAuth } from '../hooks/useAuth';

export default function DashboardPage() {
  const { user } = useAuth();
  const [stats, setStats] = useState({
    inventoryItems: 0,
    recipes: 0,
    plannedMeals: 0,
    expiringItems: 0,
  });

  useEffect(() => {
    // Fetch dashboard stats
    // TODO: Replace with actual API call
    setStats({
      inventoryItems: 45,
      recipes: 12,
      plannedMeals: 7,
      expiringItems: 3,
    });
  }, []);

  return (
    <div className="space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-gray-900">
          Hola, {user?.name?.split(' ')[0] || 'Usuario'}
        </h1>
        <p className="text-gray-600 mt-1">
          Aquí está el resumen de tu planificación de comidas
        </p>
      </div>

      {/* Stats Grid */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        <StatCard
          title="Inventario"
          value={stats.inventoryItems}
          subtitle="items disponibles"
          icon={<Package className="h-8 w-8" />}
          color="bg-blue-500"
          link="/inventory"
        />
        <StatCard
          title="Recetas"
          value={stats.recipes}
          subtitle="guardadas"
          icon={<ChefHat className="h-8 w-8" />}
          color="bg-green-500"
          link="/recipes"
        />
        <StatCard
          title="Comidas Planeadas"
          value={stats.plannedMeals}
          subtitle="esta semana"
          icon={<Calendar className="h-8 w-8" />}
          color="bg-purple-500"
          link="/meal-plan"
        />
        <StatCard
          title="Por Vencer"
          value={stats.expiringItems}
          subtitle="próximos 3 días"
          icon={<AlertCircle className="h-8 w-8" />}
          color="bg-red-500"
          link="/inventory"
        />
      </div>

      {/* Quick Actions */}
      <div className="card">
        <h2 className="text-xl font-semibold mb-4">Acciones Rápidas</h2>
        <div className="grid grid-cols-1 md:grid-cols-3 gap-4">
          <Link
            to="/inventory"
            className="flex items-center space-x-3 p-4 rounded-lg border-2 border-gray-200 hover:border-primary-500 hover:bg-primary-50 transition-colors"
          >
            <Package className="h-6 w-6 text-primary-600" />
            <div>
              <div className="font-medium">Agregar Alimentos</div>
              <div className="text-sm text-gray-500">Actualiza tu inventario</div>
            </div>
          </Link>
          <Link
            to="/recipes"
            className="flex items-center space-x-3 p-4 rounded-lg border-2 border-gray-200 hover:border-primary-500 hover:bg-primary-50 transition-colors"
          >
            <ChefHat className="h-6 w-6 text-primary-600" />
            <div>
              <div className="font-medium">Generar Receta</div>
              <div className="text-sm text-gray-500">Con IA basada en inventario</div>
            </div>
          </Link>
          <Link
            to="/meal-plan"
            className="flex items-center space-x-3 p-4 rounded-lg border-2 border-gray-200 hover:border-primary-500 hover:bg-primary-50 transition-colors"
          >
            <Calendar className="h-6 w-6 text-primary-600" />
            <div>
              <div className="font-medium">Planear Semana</div>
              <div className="text-sm text-gray-500">Organiza tus comidas</div>
            </div>
          </Link>
        </div>
      </div>

      {/* Today's Meals */}
      <div className="card">
        <div className="flex items-center justify-between mb-4">
          <h2 className="text-xl font-semibold">Comidas de Hoy</h2>
          <Link to="/meal-plan" className="text-primary-600 hover:text-primary-700 text-sm font-medium">
            Ver todas
          </Link>
        </div>
        <div className="space-y-3">
          <MealCard
            type="Desayuno"
            time="08:00"
            name="Tostadas con aguacate"
            calories="350 kcal"
          />
          <MealCard
            type="Almuerzo"
            time="13:00"
            name="Pollo al horno con vegetales"
            calories="520 kcal"
          />
          <MealCard
            type="Cena"
            time="20:00"
            name="Ensalada mediterránea"
            calories="380 kcal"
          />
        </div>
      </div>

      {/* Recommendations */}
      <div className="card">
        <div className="flex items-center space-x-2 mb-4">
          <TrendingUp className="h-5 w-5 text-primary-600" />
          <h2 className="text-xl font-semibold">Recomendaciones</h2>
        </div>
        <ul className="space-y-2">
          <li className="flex items-start space-x-2 text-sm">
            <AlertCircle className="h-5 w-5 text-orange-500 flex-shrink-0 mt-0.5" />
            <span>Tienes tomates que vencen en 2 días. Te sugerimos hacer salsa de tomate.</span>
          </li>
          <li className="flex items-start space-x-2 text-sm">
            <AlertCircle className="h-5 w-5 text-blue-500 flex-shrink-0 mt-0.5" />
            <span>Para tu plan de la próxima semana necesitas comprar: leche, huevos y pan.</span>
          </li>
          <li className="flex items-start space-x-2 text-sm">
            <AlertCircle className="h-5 w-5 text-green-500 flex-shrink-0 mt-0.5" />
            <span>Tienes suficientes ingredientes para hacer pasta carbonara.</span>
          </li>
        </ul>
      </div>
    </div>
  );
}

function StatCard({
  title,
  value,
  subtitle,
  icon,
  color,
  link
}: {
  title: string;
  value: number;
  subtitle: string;
  icon: React.ReactNode;
  color: string;
  link: string;
}) {
  return (
    <Link to={link} className="card hover:shadow-lg transition-shadow">
      <div className="flex items-start justify-between">
        <div>
          <p className="text-gray-600 text-sm">{title}</p>
          <p className="text-3xl font-bold mt-2">{value}</p>
          <p className="text-gray-500 text-sm mt-1">{subtitle}</p>
        </div>
        <div className={`${color} text-white p-3 rounded-lg`}>
          {icon}
        </div>
      </div>
    </Link>
  );
}

function MealCard({
  type,
  time,
  name,
  calories
}: {
  type: string;
  time: string;
  name: string;
  calories: string;
}) {
  return (
    <div className="flex items-center justify-between p-4 rounded-lg bg-gray-50">
      <div className="flex items-center space-x-3">
        <div className="flex items-center justify-center h-10 w-10 rounded-full bg-primary-100">
          <Clock className="h-5 w-5 text-primary-600" />
        </div>
        <div>
          <div className="font-medium">{name}</div>
          <div className="text-sm text-gray-500">{type} • {time}</div>
        </div>
      </div>
      <div className="text-sm font-medium text-gray-700">{calories}</div>
    </div>
  );
}

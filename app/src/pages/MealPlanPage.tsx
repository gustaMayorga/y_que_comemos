import { useState } from 'react';
import { Calendar, ChevronLeft, ChevronRight, Plus, Sparkles } from 'lucide-react';
import { format, startOfWeek, addDays, isSameDay } from 'date-fns';
import { es } from 'date-fns/locale';

export default function MealPlanPage() {
  const [currentDate, setCurrentDate] = useState(new Date());
  const [selectedDate, setSelectedDate] = useState(new Date());
  const [showAddMeal, setShowAddMeal] = useState(false);

  const weekStart = startOfWeek(currentDate, { weekStartsOn: 1 });
  const weekDays = Array.from({ length: 7 }, (_, i) => addDays(weekStart, i));

  const handlePreviousWeek = () => {
    setCurrentDate(addDays(currentDate, -7));
  };

  const handleNextWeek = () => {
    setCurrentDate(addDays(currentDate, 7));
  };

  const handleAutoGenerate = () => {
    // TODO: Call AI API to auto-generate meal plan
  };

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center md:justify-between gap-4">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Plan de Comidas</h1>
          <p className="text-gray-600 mt-1">Organiza las comidas de tu familia</p>
        </div>

        <button
          onClick={handleAutoGenerate}
          className="btn-primary flex items-center space-x-2"
        >
          <Sparkles className="h-4 w-4" />
          <span>Generar Plan Automático</span>
        </button>
      </div>

      {/* Week Navigator */}
      <div className="card">
        <div className="flex items-center justify-between mb-4">
          <button
            onClick={handlePreviousWeek}
            className="p-2 hover:bg-gray-100 rounded-lg"
          >
            <ChevronLeft className="h-5 w-5" />
          </button>

          <h2 className="text-lg font-semibold">
            {format(weekStart, 'd MMM', { locale: es })} -{' '}
            {format(addDays(weekStart, 6), 'd MMM yyyy', { locale: es })}
          </h2>

          <button
            onClick={handleNextWeek}
            className="p-2 hover:bg-gray-100 rounded-lg"
          >
            <ChevronRight className="h-5 w-5" />
          </button>
        </div>

        {/* Week View */}
        <div className="grid grid-cols-7 gap-2">
          {weekDays.map((day) => (
            <button
              key={day.toISOString()}
              onClick={() => setSelectedDate(day)}
              className={`
                p-3 rounded-lg text-center transition-colors
                ${
                  isSameDay(day, selectedDate)
                    ? 'bg-primary-600 text-white'
                    : isSameDay(day, new Date())
                    ? 'bg-primary-100 text-primary-700'
                    : 'hover:bg-gray-100'
                }
              `}
            >
              <div className="text-xs font-medium mb-1">
                {format(day, 'EEE', { locale: es })}
              </div>
              <div className="text-lg font-bold">{format(day, 'd')}</div>
              <div className="text-xs mt-1 opacity-75">3 comidas</div>
            </button>
          ))}
        </div>
      </div>

      {/* Selected Day Meals */}
      <div className="space-y-4">
        <div className="flex items-center justify-between">
          <h3 className="text-lg font-semibold">
            {format(selectedDate, "EEEE, d 'de' MMMM", { locale: es })}
          </h3>
          <button
            onClick={() => setShowAddMeal(true)}
            className="btn-outline text-sm flex items-center space-x-1"
          >
            <Plus className="h-4 w-4" />
            <span>Agregar Comida</span>
          </button>
        </div>

        <div className="space-y-3">
          <MealCard
            type="Desayuno"
            time="08:00"
            recipeName="Tostadas con aguacate"
            calories={350}
            members={['Juan', 'María']}
          />
          <MealCard
            type="Almuerzo"
            time="13:00"
            recipeName="Pollo al horno con vegetales"
            calories={520}
            members={['Juan', 'María', 'Pedro']}
          />
          <MealCard
            type="Cena"
            time="20:00"
            recipeName="Ensalada mediterránea"
            calories={380}
            members={['Juan', 'María']}
          />
        </div>
      </div>

      {/* Nutritional Summary */}
      <div className="card">
        <h3 className="text-lg font-semibold mb-4">Resumen Nutricional del Día</h3>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          <div>
            <div className="text-sm text-gray-600">Calorías</div>
            <div className="text-2xl font-bold mt-1">1,250</div>
            <div className="text-xs text-gray-500">de 2,000 kcal</div>
          </div>
          <div>
            <div className="text-sm text-gray-600">Proteínas</div>
            <div className="text-2xl font-bold mt-1">75g</div>
            <div className="text-xs text-gray-500">de 100g</div>
          </div>
          <div>
            <div className="text-sm text-gray-600">Carbohidratos</div>
            <div className="text-2xl font-bold mt-1">140g</div>
            <div className="text-xs text-gray-500">de 250g</div>
          </div>
          <div>
            <div className="text-sm text-gray-600">Grasas</div>
            <div className="text-2xl font-bold mt-1">45g</div>
            <div className="text-xs text-gray-500">de 70g</div>
          </div>
        </div>
      </div>

      {/* Shopping List */}
      <div className="card">
        <h3 className="text-lg font-semibold mb-4">Lista de Compras Sugerida</h3>
        <p className="text-sm text-gray-600 mb-4">
          Para completar tu plan de la semana, necesitas comprar:
        </p>
        <div className="space-y-2">
          {[
            { item: 'Leche', quantity: '2 L' },
            { item: 'Huevos', quantity: '12 unidades' },
            { item: 'Pan integral', quantity: '1 paquete' },
            { item: 'Pollo', quantity: '1 kg' },
          ].map((item, index) => (
            <div key={index} className="flex items-center justify-between p-3 bg-gray-50 rounded-lg">
              <div className="flex items-center space-x-3">
                <input type="checkbox" className="rounded border-gray-300" />
                <span>{item.item}</span>
              </div>
              <span className="text-sm text-gray-600">{item.quantity}</span>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}

function MealCard({
  type,
  time,
  recipeName,
  calories,
  members,
}: {
  type: string;
  time: string;
  recipeName: string;
  calories: number;
  members: string[];
}) {
  return (
    <div className="card hover:shadow-md transition-shadow">
      <div className="flex items-start justify-between">
        <div className="flex-1">
          <div className="flex items-center space-x-2 mb-2">
            <Calendar className="h-4 w-4 text-primary-600" />
            <span className="text-sm font-medium text-primary-600">{type}</span>
            <span className="text-sm text-gray-500">• {time}</span>
          </div>
          <h4 className="font-semibold text-lg mb-2">{recipeName}</h4>
          <div className="flex items-center space-x-4 text-sm text-gray-600">
            <span>{calories} kcal</span>
            <span>•</span>
            <span>{members.length} personas: {members.join(', ')}</span>
          </div>
        </div>
        <button className="text-gray-400 hover:text-gray-600">
          <svg className="h-5 w-5" fill="currentColor" viewBox="0 0 20 20">
            <path d="M10 6a2 2 0 110-4 2 2 0 010 4zM10 12a2 2 0 110-4 2 2 0 010 4zM10 18a2 2 0 110-4 2 2 0 010 4z" />
          </svg>
        </button>
      </div>
    </div>
  );
}

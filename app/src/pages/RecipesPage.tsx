import { useState } from 'react';
import {
  Sparkles,
  ChefHat,
  Clock,
  Users,
  Search,
  Heart,
  BookmarkPlus,
  Flame
} from 'lucide-react';
import { toast } from 'sonner';
import { Recipe, RecipeDifficulty } from '../types';

export default function RecipesPage() {
  const [recipes, setRecipes] = useState<Recipe[]>([]);
  const [showGenerator, setShowGenerator] = useState(false);
  const [generating, setGenerating] = useState(false);
  const [searchTerm, setSearchTerm] = useState('');
  const [selectedDifficulty, setSelectedDifficulty] = useState<string>('all');

  const handleGenerateRecipe = async (preferences: RecipePreferences) => {
    setGenerating(true);
    try {
      // TODO: Call AI API to generate recipe
      toast.loading('Generando recetas con IA...');

      // Simulated API call
      await new Promise(resolve => setTimeout(resolve, 2000));

      toast.success('¡3 recetas generadas!');
      setShowGenerator(false);
    } catch (error) {
      toast.error('Error al generar recetas');
    } finally {
      setGenerating(false);
    }
  };

  const filteredRecipes = recipes.filter(recipe => {
    const matchesSearch = recipe.name.toLowerCase().includes(searchTerm.toLowerCase());
    const matchesDifficulty = selectedDifficulty === 'all' || recipe.difficulty === selectedDifficulty;
    return matchesSearch && matchesDifficulty;
  });

  return (
    <div className="space-y-6">
      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center md:justify-between gap-4">
        <div>
          <h1 className="text-3xl font-bold text-gray-900">Recetas</h1>
          <p className="text-gray-600 mt-1">Descubre y genera recetas personalizadas</p>
        </div>

        <button
          onClick={() => setShowGenerator(true)}
          className="btn-primary flex items-center space-x-2"
        >
          <Sparkles className="h-4 w-4" />
          <span>Generar con IA</span>
        </button>
      </div>

      {/* Search and Filters */}
      <div className="flex flex-col md:flex-row gap-4">
        <div className="relative flex-1">
          <Search className="absolute left-3 top-1/2 transform -translate-y-1/2 h-5 w-5 text-gray-400" />
          <input
            type="text"
            placeholder="Buscar recetas..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="input pl-10"
          />
        </div>

        <select
          value={selectedDifficulty}
          onChange={(e) => setSelectedDifficulty(e.target.value)}
          className="input md:w-48"
        >
          <option value="all">Todas las dificultades</option>
          <option value="easy">Fácil</option>
          <option value="medium">Media</option>
          <option value="hard">Difícil</option>
        </select>
      </div>

      {/* Recipe Generator Feature Card */}
      <div className="card bg-gradient-to-r from-primary-50 to-primary-100 border-primary-200">
        <div className="flex items-start space-x-4">
          <div className="h-12 w-12 bg-primary-600 rounded-lg flex items-center justify-center flex-shrink-0">
            <Sparkles className="h-6 w-6 text-white" />
          </div>
          <div className="flex-1">
            <h3 className="font-semibold text-lg mb-1">Generador de Recetas con IA</h3>
            <p className="text-gray-700 text-sm mb-3">
              Crea recetas personalizadas basadas en los alimentos disponibles en tu inventario,
              las preferencias de tu familia y tus objetivos nutricionales.
            </p>
            <button
              onClick={() => setShowGenerator(true)}
              className="btn-primary text-sm"
            >
              Generar Recetas
            </button>
          </div>
        </div>
      </div>

      {/* Recipes Grid */}
      {filteredRecipes.length === 0 ? (
        <div className="card text-center py-12">
          <ChefHat className="h-16 w-16 text-gray-300 mx-auto mb-4" />
          <h3 className="text-lg font-semibold text-gray-900 mb-2">
            No hay recetas aún
          </h3>
          <p className="text-gray-600 mb-6">
            Genera tu primera receta con IA basada en tu inventario
          </p>
          <button
            onClick={() => setShowGenerator(true)}
            className="btn-primary"
          >
            Generar Recetas con IA
          </button>
        </div>
      ) : (
        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-6">
          {filteredRecipes.map((recipe) => (
            <RecipeCard key={recipe.id} recipe={recipe} />
          ))}
        </div>
      )}

      {/* Generator Modal */}
      {showGenerator && (
        <RecipeGeneratorModal
          onClose={() => setShowGenerator(false)}
          onGenerate={handleGenerateRecipe}
          loading={generating}
        />
      )}
    </div>
  );
}

function RecipeCard({ recipe }: { recipe: Recipe }) {
  const difficultyColors = {
    easy: 'bg-green-100 text-green-800',
    medium: 'bg-yellow-100 text-yellow-800',
    hard: 'bg-red-100 text-red-800',
  };

  return (
    <div className="card hover:shadow-lg transition-shadow">
      {recipe.image ? (
        <img
          src={recipe.image}
          alt={recipe.name}
          className="w-full h-48 object-cover rounded-lg mb-4"
        />
      ) : (
        <div className="w-full h-48 bg-gradient-to-br from-primary-100 to-primary-200 rounded-lg mb-4 flex items-center justify-center">
          <ChefHat className="h-16 w-16 text-primary-600" />
        </div>
      )}

      <div className="space-y-3">
        <div>
          <div className="flex items-start justify-between mb-2">
            <h3 className="font-semibold text-lg">{recipe.name}</h3>
            <button className="p-1 hover:text-red-600">
              <Heart className="h-5 w-5" />
            </button>
          </div>
          <p className="text-sm text-gray-600 line-clamp-2">{recipe.description}</p>
        </div>

        <div className="flex items-center gap-4 text-sm text-gray-600">
          <div className="flex items-center space-x-1">
            <Clock className="h-4 w-4" />
            <span>{recipe.prepTime + recipe.cookTime} min</span>
          </div>
          <div className="flex items-center space-x-1">
            <Users className="h-4 w-4" />
            <span>{recipe.servings} pers</span>
          </div>
          <div className="flex items-center space-x-1">
            <Flame className="h-4 w-4" />
            <span>{recipe.nutritionalInfo.calories} kcal</span>
          </div>
        </div>

        <div className="flex items-center justify-between pt-2">
          <span className={`px-2 py-1 rounded-full text-xs font-medium ${difficultyColors[recipe.difficulty]}`}>
            {recipe.difficulty === 'easy' ? 'Fácil' : recipe.difficulty === 'medium' ? 'Media' : 'Difícil'}
          </span>
          <button className="btn-outline text-sm flex items-center space-x-1">
            <BookmarkPlus className="h-4 w-4" />
            <span>Guardar</span>
          </button>
        </div>
      </div>
    </div>
  );
}

interface RecipePreferences {
  servings: number;
  difficulty: string;
  mealType: string;
  dietaryRestrictions: string[];
  maxTime: number;
  useInventory: boolean;
}

function RecipeGeneratorModal({
  onClose,
  onGenerate,
  loading,
}: {
  onClose: () => void;
  onGenerate: (preferences: RecipePreferences) => void;
  loading: boolean;
}) {
  const [preferences, setPreferences] = useState<RecipePreferences>({
    servings: 4,
    difficulty: 'all',
    mealType: 'lunch',
    dietaryRestrictions: [],
    maxTime: 60,
    useInventory: true,
  });

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    onGenerate(preferences);
  };

  return (
    <div className="fixed inset-0 bg-black bg-opacity-50 flex items-center justify-center z-50 p-4">
      <div className="bg-white rounded-lg max-w-2xl w-full p-6 max-h-[90vh] overflow-y-auto">
        <div className="flex items-center space-x-3 mb-6">
          <div className="h-10 w-10 bg-primary-600 rounded-lg flex items-center justify-center">
            <Sparkles className="h-6 w-6 text-white" />
          </div>
          <div>
            <h2 className="text-xl font-bold">Generar Recetas con IA</h2>
            <p className="text-sm text-gray-600">Personaliza tus preferencias</p>
          </div>
        </div>

        <form onSubmit={handleSubmit} className="space-y-6">
          <div>
            <label className="flex items-center space-x-2 mb-3">
              <input
                type="checkbox"
                checked={preferences.useInventory}
                onChange={(e) => setPreferences({ ...preferences, useInventory: e.target.checked })}
                className="rounded border-gray-300"
              />
              <span className="text-sm font-medium">Usar solo alimentos de mi inventario</span>
            </label>
            <p className="text-xs text-gray-500 ml-6">
              Las recetas se generarán usando los ingredientes que tienes disponibles
            </p>
          </div>

          <div className="grid grid-cols-2 gap-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Porciones
              </label>
              <input
                type="number"
                value={preferences.servings}
                onChange={(e) => setPreferences({ ...preferences, servings: Number(e.target.value) })}
                className="input"
                min="1"
                max="12"
              />
            </div>

            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Tiempo máximo (min)
              </label>
              <input
                type="number"
                value={preferences.maxTime}
                onChange={(e) => setPreferences({ ...preferences, maxTime: Number(e.target.value) })}
                className="input"
                min="15"
                step="15"
              />
            </div>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Tipo de comida
            </label>
            <select
              value={preferences.mealType}
              onChange={(e) => setPreferences({ ...preferences, mealType: e.target.value })}
              className="input"
            >
              <option value="breakfast">Desayuno</option>
              <option value="lunch">Almuerzo</option>
              <option value="dinner">Cena</option>
              <option value="snack">Snack</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">
              Dificultad
            </label>
            <select
              value={preferences.difficulty}
              onChange={(e) => setPreferences({ ...preferences, difficulty: e.target.value })}
              className="input"
            >
              <option value="all">Cualquiera</option>
              <option value="easy">Fácil</option>
              <option value="medium">Media</option>
              <option value="hard">Difícil</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-gray-700 mb-2">
              Restricciones dietéticas
            </label>
            <div className="grid grid-cols-2 gap-2">
              {['Vegetariano', 'Vegano', 'Sin gluten', 'Sin lactosa', 'Bajo en sodio', 'Diabético'].map((option) => (
                <label key={option} className="flex items-center space-x-2">
                  <input
                    type="checkbox"
                    checked={preferences.dietaryRestrictions.includes(option)}
                    onChange={(e) => {
                      if (e.target.checked) {
                        setPreferences({
                          ...preferences,
                          dietaryRestrictions: [...preferences.dietaryRestrictions, option],
                        });
                      } else {
                        setPreferences({
                          ...preferences,
                          dietaryRestrictions: preferences.dietaryRestrictions.filter((r) => r !== option),
                        });
                      }
                    }}
                    className="rounded border-gray-300"
                  />
                  <span className="text-sm">{option}</span>
                </label>
              ))}
            </div>
          </div>

          <div className="flex space-x-3 pt-4 border-t">
            <button
              type="button"
              onClick={onClose}
              disabled={loading}
              className="btn-secondary flex-1"
            >
              Cancelar
            </button>
            <button
              type="submit"
              disabled={loading}
              className="btn-primary flex-1 flex items-center justify-center space-x-2"
            >
              {loading ? (
                <>
                  <div className="animate-spin rounded-full h-4 w-4 border-b-2 border-white"></div>
                  <span>Generando...</span>
                </>
              ) : (
                <>
                  <Sparkles className="h-4 w-4" />
                  <span>Generar Recetas</span>
                </>
              )}
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}

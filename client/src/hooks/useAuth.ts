import { create } from "zustand";
import axios from "axios";

type AuthState = {
  user: object | null;
  loading: boolean;
  error: string | null;
  login: (email: string, password: string) => Promise<void>;
  logout: () => void;
  setUser: (user: object | null) => void;
  clearError: () => void;
  checkAuth: () => void;
};

export const useAuth = create<AuthState>((set) => ({
  user: null,
  loading: false,
  error: null,
  setUser: (user) => set({ user }),

  async login(email, password) {
    set({ loading: true, error: null });
    try {
      const response = await axios.post(
        "/api/auth/login",
        { email, password },
        { withCredentials: true }
      );
      if (response.data.error) {
        set({ error: response.data.error, loading: false });
      } else {
        set({ user: response.data || null, loading: false });
      }
    } catch (err) {
      set({
        error: err instanceof Error ? err.message : "An unknown error occurred",
        loading: false,
      });
    }
  },

  logout() {
    set({ user: null, error: null });
    axios
      .delete("/api/auth/logout", { withCredentials: true })
      .catch(console.error);
    window.location.reload();
  },

  clearError: () => set({ error: null }),

  checkAuth() {
    set({ loading: true, error: null });
    axios
      .get("/api/auth/me", { withCredentials: true })
      .then((response) => {
        if (response.data.error) {
          set({ error: response.data.error, loading: false });
        } else {
          set({ user: response.data || null, loading: false });
        }
      })
      .catch((err) => {
        set({
          error:
            err instanceof Error ? err.message : "An unknown error occurred",
          loading: false,
        });
      });
  },
}));

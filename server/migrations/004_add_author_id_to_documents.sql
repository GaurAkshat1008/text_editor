
ALTER TABLE documents ADD COLUMN author_id INTEGER;
ALTER TABLE documents ADD CONSTRAINT fk_author FOREIGN KEY (author_id) REFERENCES authors(id);